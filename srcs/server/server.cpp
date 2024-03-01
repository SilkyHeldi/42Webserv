#include "../config/config.hpp"
#include "../HttpRequest/HttpRequest.hpp"
#include "server.hpp"

Server::Server(void):
	_efd(0),
	_port_list(),
	_requests(),
	_events(NULL),
	_sfds()
{
}

Server::Server(Server const &rhs):
	_efd(rhs._efd),
	_port_list(rhs._port_list),
	_requests(rhs._requests),
	_events(rhs._events),
	_sfds(rhs._sfds)
{
}

Server::~Server(void)
{
  close(_efd);
}

Server	&Server::operator=(Server const &rhs)
{
	_efd = rhs._efd;
	_port_list = rhs._port_list;
	_requests = rhs._requests;
	_events = rhs._events;
	_sfds = rhs._sfds;
	return (*this);
}

/**************************************SERVER SETUP**************************************/
int Server::get_port_from_fd(int sockfd) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    if (getsockname(sockfd, (struct sockaddr*)&addr, &addr_len) == -1) {
        perror("getsockname");
        return (-1);
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        return ntohs(s->sin_port);
    } else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        return ntohs(s->sin6_port);
    } else {
        std::cerr << "Unsupported address family" << std::endl;
        return (-1);
    }
}

int	Server::create_and_bind (char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s = 0;
  int sfd = 0;

  /*Dgive IP criteria*/
  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* choix en IPv4 et IPv6 */
  hints.ai_socktype = SOCK_STREAM; // TCP socket
  hints.ai_flags = AI_PASSIVE;// all interfaces

  s = getaddrinfo (NULL, port, &hints, &result);//compatible IP list in **result
  if (s != 0)
  {
    std::cerr << "Error : getaddrinfo : " << gai_strerror(s) << std::endl;
    freeaddrinfo(result);
    return(-1);
  }
  //go through IP list, try socket + try bind
  for (rp = result; rp != NULL; rp = rp->ai_next)
  {
    sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1)
      continue;
    s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
    if (s == 0)
       break;// bind success
    close(sfd);
  }
  if (rp == NULL)
  {
    std::cerr << "Could not bind" << std::endl;
    freeaddrinfo(result);
    return(-1);
  }
  freeaddrinfo(result);
  return(sfd);
}


int	Server::make_socket_non_blocking (int sfd)
{
  int flags = 0;
  int s = 0;

  flags = fcntl (sfd, F_GETFL, 0);//get current flags
  if (flags == -1)
  {
    perror("fcntl");
    return(-1);
  }
  flags |= O_NONBLOCK;
  s = fcntl(sfd, F_SETFL, flags);
  if (s == -1)
  {
    perror("fcntl");
    return(-1);
  }
  return(0);
}

int Server::init_sockets(Config_List servlist)
{
  std::vector<std::string>  distinctPorts = servlist.getDistinctPorts();
  for (std::vector<std::string>::iterator it = distinctPorts.begin(); it != distinctPorts.end(); ++it)
  {
    _sfds.push_back(create_and_bind(const_cast<char*>((*it).c_str())));
    if (_sfds.back() == -1)
      return(0);

    /*l'appel à une fonction de lecture ou d'écriture sur une socket
    bloquante pourrait bloquer l'ensemble du programme*/
    int s = make_socket_non_blocking (_sfds.back());
    if (s == -1)
      return(0);
  
    /*s ecoute connexions entrantes, SOMAXCONN = taille max file d'attente*/
    s = listen (_sfds.back(), SOMAXCONN);
    if (s == -1)
    {
      perror ("listen");
      return(0);
    }
  }
  return(1);
}

int Server::init_epoll(struct epoll_event &event)
{
  int s = 0;

  /** objet epoll, qui sera utilisé pour surveiller les événements sur
 plusieurs descripteurs de fichiers, y compris les sockets **/
  _efd = epoll_create1 (0);
  if (_efd == -1)
  {
    perror("epoll_create");
    return(0);
  }

  for (std::vector<int>::iterator it = _sfds.begin(); it != _sfds.end(); ++it) //ajout d'un fd par port
  {
    event.data.fd = *it;
    event.events = EPOLLIN | EPOLLOUT;
    s = epoll_ctl(_efd, EPOLL_CTL_ADD, *it, &event);
    if (s == -1)
    {
      perror("epoll_init : epoll_ctl");
      return(0);
    }
  }
  return(1);
}

std::vector<HttpRequest>::iterator  Server::search_for_req(int fd)
{
  // search for right fd and its HttpRequest instance
  std::vector<HttpRequest>::iterator it;
  for (it = _requests.begin(); it != _requests.end(); ++it)
  {
    //std:: cout << it->get_fd() << " == " << fd << "?" << std::endl;
    if (it->get_fd() == fd)
      return(it);
  }
  //fd not found in existing requests fds
  it->set_done(1);
  std::cerr << "Error : " <<RED<<"500"<<DEFAULT<< " : non existant fd" << std::endl;
  std::string body = readFileIntoString("site/errors/500.html");
  if (body.empty())
      std::cerr << "Error 500 : Failed to open file : " << "site/errors/500.html" << std::endl;
  
  std::string	response;
  it->prepareResponse(body, "500", "Internal Server Error", response);
  int bytes_sent = send(fd, response.c_str(), response.size(), 0);
  if (bytes_sent == -1)
  {
    std::cerr << "Error : response bytes could not be sent (3)" << std::endl;
  }
  return (it);
}

//global variable to check ctrl+c
volatile sig_atomic_t stopFlag = 0;

void signalHandler(int signum)
{
  (void)signum;
  stopFlag = 1; // Définir le drapeau pour indiquer l'arrêt du serveur
}

/*********************************************START SERVER***********************************************/
int Server::happen_serv_check(int i)
{
  bool  hasErrorOrUp = (_events[i].events & (EPOLLERR | EPOLLHUP));
  bool  notReadyForIO = !(_events[i].events & (EPOLLIN | EPOLLOUT));
	if (hasErrorOrUp || notReadyForIO)
	{
        /* An error has occured on this fd, or the socket is not
           ready for reading (why were we notified then?) */
	  std::cerr << "Error : epoll on descriptor " << _events[i].data.fd << std::endl;
	  close(_events[i].data.fd);
	  return(1);
	}
  return(-1);
}

int Server::new_connection(std::vector<int>::iterator sfd, struct epoll_event &event)
{
  for (;;)
  {
    struct sockaddr in_addr;
    socklen_t in_len;
    int infd = 0;
    char hbuf[NI_MAXHOST] = {0}, sbuf[NI_MAXSERV] = {0};
    in_len = sizeof in_addr;

    infd = accept (*sfd, &in_addr, &in_len);
    if (infd == -1)
    {
      if ((errno == EAGAIN) ||//asked resource not av atm
          (errno == EWOULDBLOCK))//operation would block if block-mode
        break; // We have processed all incoming connections
      else
      {
        perror ("accept");
        break;
      }
    }
    //convert IP and port binary->string
    int s = getnameinfo (&in_addr, in_len,
                     hbuf, sizeof hbuf,
                     sbuf, sizeof sbuf,
                     NI_NUMERICHOST | NI_NUMERICSERV);
    if (s == 0)
    {
      std::cout << "Accepted connection on descriptor " << infd;
      std::cout << " (host=" << hbuf <<  ", port=" << sbuf << ")" << std::endl;
    }
    /* Make the incoming socket non-blocking and add it to the
       list of fds to monitor. */
    s = make_socket_non_blocking (infd);
    if (s == -1) {
      free(_events);
      return(-1);
    }
    event.data.fd = infd;
    event.events = EPOLLIN;
    s = epoll_ctl (_efd, EPOLL_CTL_ADD, infd, &event);
    if (s == -1)
    {
      perror("new connection : epoll_ctl");
      free(_events);
      return(-1);
    }
    HttpRequest requestset;
    requestset.set_port(get_port_from_fd(*sfd));
    requestset.set_fd(infd);
    requestset.set_currentTime();
    _requests.push_back(requestset);
  }
  return(1);
}

std::vector<HttpRequest>::iterator  Server::get_matching_request(int fd)
{
  std::vector<HttpRequest>::iterator  it;
  try {
    it = search_for_req(fd);
    return(it);
  }
  catch (std::exception &e) {
    DefaultError  err;
    err.manageError(*it, e, fd);
	  it->set_done(1);
    return(it);
  }
}

void Server::recv_request(Config_List servlist, int fd, std::vector<HttpRequest>::iterator it, int i)
{
    bool shouldReceive = !it->get_recv() && !(_events[i].events & EPOLLOUT) && !it->get_done();

    if (shouldReceive)
    {
        try {
            it->reqcv(fd, servlist);
        }
        catch (std::exception &e) {
            DefaultError  err;
            err.manageError(*it, e, _events[i].data.fd);
            it->set_done(1);
        }
    }
}

int  Server::change_of_state(std::vector<HttpRequest>::iterator it, struct epoll_event &event, int i)
{
  if (it->isComplete() && (!(_events[i].events & EPOLLOUT)) && it->get_done() == false)
  {
    it->set_recv(1);
    //std::cout << *it << std::endl;
    event.data.fd = _events[i].data.fd;
    event.events = EPOLLOUT;
    int s = epoll_ctl(_efd, EPOLL_CTL_MOD, _events[i].data.fd, &event);
    if (s == -1)
    {
      perror ("EPOLLOUT change : epoll_ctl");
      return(-1);
    }
    return(1);
  }
  return(0);
}

void Server::write_response(std::vector<HttpRequest>::iterator it, Config_List servlist, int i)
{
    if ((_events[i].events & EPOLLOUT) && !it->get_done())
    {
      if (it->get_method() == "GET" && isDownloadRequest(*it))
          it->sendDownload("200", _events[i].data.fd);

      else if (it->get_method() == "GET" || it->get_method() == "DELETE")
          it->sendResponse("200", _events[i].data.fd);
  
      else if (it->get_method() == "POST")
      {
          try {
              it->post_request(_events[i].data.fd, servlist);
          }
          catch (std::exception &e) {
              handlePostRequestError(*it, e, _events[i].data.fd);
              it->set_done(1);
          }
      }
    }
}

bool Server::isDownloadRequest(HttpRequest &request)
{
    return request.get_uri().compare(0, 10, "/downloads") == 0;
}

void Server::handlePostRequestError(HttpRequest &request, std::exception &e, int fd)
{
    DefaultError err;
    err.manageError(request, e, fd);
}


/*********************************************MAIN FUNCTION**************************************************/

int Server::start_server(Config_List servlist)
{
    // Initialize signal handler
    signal(SIGINT, signalHandler);

    // Initialize sockets and epoll
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    if (!init_sockets(servlist) || !init_epoll(event))
        return -1;

    // Main server loop
    while (!stopFlag)
    {
        _events = (struct epoll_event *)calloc(MAXEVENTS, sizeof event); // Buffer for events
        int n = epoll_wait(_efd, _events, MAXEVENTS, -1);
        for (int i = 0; i < n; i++)
        {
            if (happen_serv_check(i) == 1)
                continue;

            std::vector<int>::iterator sfd = std::find(_sfds.begin(), _sfds.end(), _events[i].data.fd);
            if (sfd != _sfds.end() && *sfd == _events[i].data.fd) // New connection
            {
                if (new_connection(sfd, event) == -1)
                    return -1;
                continue;
            }
            else // Read or write
            {
              std::vector<HttpRequest>::iterator it = get_matching_request(_events[i].data.fd);
              handle_request(servlist, it, i, event);
            }
        }
        free(_events);
    }

    // Cleanup and return
    return EXIT_SUCCESS;
}

void Server::handle_request(Config_List servlist, std::vector<HttpRequest>::iterator it, int i, struct epoll_event &event)
{
    if (it != _requests.end())
    {
        recv_request(servlist, _events[i].data.fd, it, i);
        int state = change_of_state(it, event, i);
        if (state == 1)
            return;
        else if (state == -1)
            stopFlag = true;
        write_response(it, servlist, i);
        if (it->get_done())
        {
            std::cout << "Closed connection on descriptor " << _events[i].data.fd << std::endl;
            _requests.erase(it);
            close(_events[i].data.fd);
        }
    }
}