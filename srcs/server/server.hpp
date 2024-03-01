#ifndef SERVER_HPP
# define SERVER_HPP

# define MAXEVENTS 128
# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <cstdlib> // For exit() and EXIT_FAILURE
# include <iostream> // For cout
# include <unistd.h> // For read
# include <string.h>
# include <netdb.h>
# include <fcntl.h>
# include <limits>
# include <csignal>
# include <sys/ioctl.h>
# include <sys/epoll.h>
# include "../HttpRequest/HttpRequest.hpp"
# include "../config/config.hpp"

class Server
{
	private:
		int	_efd;
  		std::vector<int>  	_port_list;
  		std::vector<HttpRequest>  _requests;
		struct epoll_event 	*_events;
		std::vector<int>  	_sfds;

		//SERVER SETUP
		int get_port_from_fd(int sockfd);
		int	create_and_bind (char *port);
		int	make_socket_non_blocking (int sfd);
		int init_sockets(Config_List servlist);
		//bool add_to_epoll(int fd, struct epoll_event &event);
		int init_epoll(struct epoll_event &event);
		std::vector<HttpRequest>::iterator  search_for_req(int fd);

		//START SERVER
		bool isDownloadRequest(HttpRequest &request);
		void handlePostRequestError(HttpRequest &request, std::exception &e, int fd);
		int happen_serv_check(int i);
		int new_connection(std::vector<int>::iterator sfd, struct epoll_event &event);
		std::vector<HttpRequest>::iterator  get_matching_request(int fd);
		void handle_request(Config_List servlist, std::vector<HttpRequest>::iterator it, int i, struct epoll_event &event);
		void recv_request(Config_List servlist, int fd, std::vector<HttpRequest>::iterator it, int i);
		int change_of_state(std::vector<HttpRequest>::iterator  it, struct epoll_event &event, int i);
		void write_response(std::vector<HttpRequest>::iterator it, Config_List servlist, int i);

	public:
		Server(void);
		Server(Server const &rhs);
		~Server(void);
		Server	&operator=(Server const &rhs);

		int start_server(Config_List servlist);
};

#endif