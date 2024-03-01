/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llepiney <llepiney@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 01:18:42 by rgarrigo          #+#    #+#             */
/*   Updated: 2023/07/29 20:12:43 by llepiney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "HttpRequest.hpp"
#include "../config/config.hpp"

/********************************************COPLIEN***********************************************/
/*************************************************************************************************/
//? Coplien
HttpRequest::HttpRequest(void):
	_currentTime(0),
	_startTime(0),
	_request(),
	_order(),
	_body(""),
	_raw(""),
	_reqlineComplete(false),
	_headerComplete(false),
	_bodyComplete(false),
	_ready(false),
	_done(false),
	_recv(false),
	_port(0),
	_fd(0),
	_count(0),
	_chunked(false),
	_oct_size(true),
	_firstChunk(true),
	send_queue(),
	_config(),
	_loc()
{
}

HttpRequest::HttpRequest(HttpRequest const &rhs):
	_currentTime(rhs._currentTime),
	_startTime(rhs._startTime),
	_request(rhs._request),
	_order(rhs._order),
	_body(rhs._body),
	_raw(rhs._raw),
	_reqlineComplete(rhs._reqlineComplete),
	_headerComplete(rhs._headerComplete),
	_bodyComplete(rhs._bodyComplete),
	_ready(rhs._ready),
	_done(rhs._done),
	_recv(rhs._recv),
	_port(rhs._port),
	_fd(rhs._fd),
	_count(rhs._count),
	_chunked(rhs._chunked),
	_oct_size(rhs._oct_size),
	_firstChunk(rhs._firstChunk),
	send_queue(rhs.send_queue),
	_config(rhs._config),
	_loc(rhs._loc)
{
}

HttpRequest::~HttpRequest(void)
{
}

HttpRequest	&HttpRequest::operator=(HttpRequest const &rhs)
{
	_currentTime = rhs._currentTime;
	_startTime = rhs._startTime;
	_request = rhs._request;
	_order = rhs._order;
	_body = rhs._body;
	_raw = rhs._raw;
	_reqlineComplete = (rhs._reqlineComplete);
	_headerComplete = rhs._headerComplete;
	_bodyComplete = rhs._bodyComplete;
	_ready = rhs._ready;
	_done = rhs._done;
	_recv = rhs._recv;
	_port = rhs._port;
	_fd = rhs._fd;
	_count = rhs._count;
	_chunked = rhs._chunked;
	_oct_size = rhs._oct_size;
	_firstChunk = rhs._firstChunk;
	send_queue = rhs.send_queue;
	_config = rhs._config;
	_loc = rhs._loc;
	return (*this);
}

//? operator<<
//? Print request
std::ostream &operator<<(std::ostream &out, const HttpRequest &rhs)
{
    out << "HTTP Request" << std::endl;
    out << rhs._request.at("Method") << " " << rhs._request.at("URI") << " " << rhs._request.at("Version") << std::endl;
    int count = 0;
    for (std::vector<std::string>::const_iterator keyIt = rhs._order.begin(); keyIt != rhs._order.end(); ++keyIt) {
        const std::string &key = *keyIt;
        if (count >= 3) {
            out << key << ": " << rhs._request.at(key) << std::endl;
        }
        count++;
    }
    out << "Body:" << std::endl;
    out << rhs._body << DEFAULT;
    return out;
}

//?Get set
std::string	HttpRequest::get_method(void)
{
	return (this->_request["Method"]);
}

std::string	HttpRequest::get_uri(void)
{
	return (this->_request["URI"]);
}

bool	HttpRequest::get_done(void)
{
	return (this->_done);
}

void	HttpRequest::set_done(int num)
{
	if (num == 0)
		this->_done = false;
	else
		this->_done = true;
}

bool	HttpRequest::get_recv(void)
{
	return (this->_recv);
}

void	HttpRequest::set_recv(int num)
{
	if (num == 0)
		this->_recv = false;
	else
		this->_recv = true;
}

bool	HttpRequest::get_ready(void)
{
	return (this->_ready);
}

void	HttpRequest::set_ready(int num)
{
	if (num == 0)
		this->_ready = false;
	else
		this->_ready = true;
}

int	HttpRequest::get_port(void)
{
	return (this->_port);
}

void	HttpRequest::set_port(int num)
{
	this->_port = num;
}

int	HttpRequest::get_fd(void)
{
	return (this->_fd);
}

void	HttpRequest::set_fd(int num)
{
	this->_fd = num;
}

int	HttpRequest::get_count(void)
{
	return (this->_count);
}

// Getter for _scriptPath
const std::string& HttpRequest::getScriptPath() const {
	return _scriptPath;
}

// Getter for _location
const std::string& HttpRequest::getLocation() const {
	return _location;
}

// Getter for _config
const Config& HttpRequest::getConfig() const {
	return _config;
}
void	HttpRequest::set_count(int num)
{
	this->_count = num;
}

std::string		HttpRequest::get_content_length(void)
{
	return (this->_request["Content-Length"]);
}

void	HttpRequest::set_currentTime(void)
{
	this->_currentTime = std::time(NULL);
	this->_startTime = std::time(NULL);
}

void print_char_array(char** arr)
{
    for (; *arr; ++arr) std::cout <<BLUE_TEXT<< *arr <<DEFAULT<< std::endl;
}


/********************************************PARSING-CHECK****************************************/
/*************************************************************************************************/

#include <ctime>

//? isComplete
//? Check if request is complete
bool	HttpRequest::isComplete(void) const
{
	return (_headerComplete && _bodyComplete);
}

//? _verifyHeader
//? If method is forbidden -> Error 405
//? If method is unknown -> Error 501
void	HttpRequest::addVerifReqLine(std::string requestLine)
{
	//REQUEST LINE SEPARATION INTO _REQUEST
	std::istringstream iss(requestLine);
    std::string method, uri, version;

    if (iss >> method >> uri >> version) {
        _request["Method"] = method;
		_order.push_back("Method");
        _request["URI"] = uri;
		_order.push_back("URI");
        _request["Version"] = version;
		_order.push_back("Version");
    }
	else
	{
        std::cerr << "Error during request line separation" << std::endl;
		throw (Error("500"));
    }

	if (_request["Version"] != "HTTP/1.1")
		throw(Error("505"));

	if ((_request["URI"]).length() > 40)
	{
		std::cerr << "Error " << RED<<"414"<<DEFAULT;
		std::cerr << " : Request-URI Too Long" << std::endl;
		throw(Error("414"));
	}
}

void	HttpRequest::reqline(std::string& _raw, size_t& crlfPos)
{
	if (crlfPos != std::string::npos && _reqlineComplete == false)
	{
   		std::string requestLine = _raw.substr(0, crlfPos);
   		_raw = _raw.substr(crlfPos + 2);
		addVerifReqLine(requestLine);
		_reqlineComplete = true;
		crlfPos = _raw.find(CRLF);
	}
}

static bool	crlfCheck(std::string _raw)
{
	if (_raw.find(CRLF) == std::string::npos)
		return(false);
	return(true);
}

void	HttpRequest::headers(std::string& _raw, int bytesRead)
{
   	std::istringstream headersStream(_raw);
   	std::string headerLine;
   	while (_reqlineComplete && !_headerComplete && crlfCheck(_raw)
			&& std::getline(headersStream, headerLine))
	{
		if (headerLine == "\r" || headerLine == "\n" || headerLine.empty())
    	{
    	    //empty line
    	    _headerComplete = true;
			_raw = _raw.substr(2);
    	    break;
    	}

		//remove treated line from _raw
		size_t linePos = _raw.find('\r');
		if (linePos != std::string::npos)
    		_raw = _raw.substr(linePos + 2);
		else
		{
			std::cerr << "Error " <<RED<<"500"<<DEFAULT << " : failed to remove";
			std::cerr << " treated line in request parsing : wrong header" << std::endl;
			throw(Error("500"));
		}

		//_request map filling
		size_t colonPos = headerLine.find(':');
		headerLine.erase(std::remove(headerLine.begin(), headerLine.end(), '\r'), headerLine.end());
    	if (colonPos != std::string::npos)
		{
    	    std::string key = headerLine.substr(0, colonPos);
    	    std::string value = headerLine.substr(colonPos + 2);
			if (key == "Transfer-Encoding" && value == "chunked")
				_chunked = true;
    	    _request[key] = value;
			_order.push_back(key);
    	}
		if (!_headerComplete && bytesRead == 0) //closed connection
		{
			std::cerr << "Error : closed connection while header not complete" << std::endl;
			throw (Error("Closed"));
		}
   	}
}

int HttpRequest::checkMethod() {
    std::vector<std::string> combinedVector;

    if (!_loc._limit_except.empty()) {
        combinedVector = _loc._limit_except;
    } else if (!_config._limit_except.empty()) {
        combinedVector = _config._limit_except;
    } else {
        combinedVector.push_back("GET");
        combinedVector.push_back("POST");
        combinedVector.push_back("DELETE");
    }

    for (std::vector<std::string>::const_iterator it = combinedVector.begin(); it != combinedVector.end(); ++it) {
        if (*it == _request["Method"]) {
            return 1;
        }
    }
	if (_request["Method"] != "GET" && _request["Method"] != "POST"
			&& _request["Method"] != "DELETE")
		return 2;
    return 0;
}

void	HttpRequest::lengthCheck(void)
{
	if (_request.find("Content-Length") != _request.end())
	{
		int	to_check = std::atoi(_request["Content-Length"].c_str());
		std::string client_max_body_size = _config.getBodyLimitForLocation(_loc._path);
		if (!client_max_body_size.empty() && to_check > std::atoi((client_max_body_size).c_str()))
			throw(Error("413"));
		else if (client_max_body_size.empty() && to_check > BODY_LIMIT)
			throw(Error("413"));
		else if (_body.length() >= static_cast<std::string::size_type>(std::atoi(_request["Content-Length"].c_str())))
			_bodyComplete = true;
	}
	else
	{
		if (_request["Method"] == "GET" || _request["Method"] == "DELETE")
			_bodyComplete = true;
		else if (!(_request["Method"] == "POST" && _request["Transfer-Encoding"] == "chunked"))
		{
			std::cerr << "Error " << RED<<"411"<<DEFAULT;
			std::cerr << " : failed to find length" << std::endl;
			throw(Error("411"));
		}
	}
}

void	HttpRequest::extensionCheck(void)
{
	if (_body.find("filename=") != std::string::npos && _request["Method"] == "POST")
	{
		std::string filename = extractFilename(_body.c_str());
		std::string fileExtension = get_file_suffix(filename);
		if (fileExtension != ".txt")
		{
			std::cerr << "Error " <<  RED<<"415"<<DEFAULT;
			std::cerr << " : Unsupported Media Type" << std::endl;
			throw(Error("415"));
		}
	}
}

void	HttpRequest::manageChunkedBody(void)
{
	std::istringstream chunkStream(_raw);
   	std::string line;
   	while (crlfCheck(_raw) && std::getline(chunkStream, line))
	{
		if (_oct_size == true && (line == "0\r" || line.empty()))
    	{
			if (line == "0\r" && _firstChunk == true)
				throw(Error("204"));
    	    _headerComplete = true;
			_bodyComplete = true;
			_raw = _raw.substr(1);
    	    break;
    	}
		if (_firstChunk == true)
			_firstChunk = false;
		//remove treated line from _raw
		size_t linePos = _raw.find('\r');
		if (linePos != std::string::npos)
    		_raw = _raw.substr(linePos + 2);
		else
		{
			std::cerr << "Error " <<RED<<"500"<<DEFAULT << " : failed to remove";
			std::cerr << " treated line in request parsing : wrong chunked body" << std::endl;
			throw(Error("500"));
		}

		if (_oct_size == true)
		{
			_oct_size = false;
			continue;
		}
		_body += line;
		_oct_size = true;
	}
}

void	HttpRequest::bodyFillCheck(int bytesRead, char *buffer, const Config_List &servlist)
{
	_raw += std::string(buffer, bytesRead);
	_count += bytesRead;
	size_t crlfPos = _raw.find(CRLF);
	reqline(_raw, crlfPos);
	headers(_raw, bytesRead);
	if (_headerComplete && _config._port == "0") {
		std::stringstream portStream;
		portStream << _port;
		_config = servlist.getConfigByPortAndServerName(portStream.str(), _request);
		// _config.display();
	}
	if (_headerComplete)
	{
		if (_bodyComplete == false)
		{
			_loc = _config.findMatchingLocation(_request["URI"]);
			int	s = checkMethod();
			if (s == 0)
				throw(Error("405"));
			if (s == 2)
				throw(Error("501"));
			if (_chunked)
				manageChunkedBody();
			else
				_body += _raw;
			lengthCheck();
		}
		extensionCheck();
	}
}

void	HttpRequest::reqcv(int fd, const Config_List &servlist)
{
	char buffer[BUFFER_SIZE] = {0};

	(void)servlist;
	int bytesRead = ::recv(fd, buffer, BUFFER_SIZE, 0);

	if (bytesRead > 0)
	    bodyFillCheck(bytesRead, buffer, servlist);
	else if (bytesRead == 0)
	{
		std::cerr << RED<<"No byte read from client" <<DEFAULT<< std::endl;
		perror("recv");
	    throw(Error("500"));
	}
	else
	{
		std::cerr << "Error " << RED<<"500"<<DEFAULT;
		std::cerr << " : failed to read from client" << std::endl;
	    throw(Error("500"));
	}

	//TIMEOUT CHECK
	_currentTime = std::time(NULL);
   	if (_currentTime - _startTime >= TIMEOUT_DURATION)
	{
		std::cerr << "Error " << RED << "408" << DEFAULT;
	    std::cerr << " : request timeout" << std::endl;
	    throw(Error("408"));
	}
}

std::string readFileIntoString(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    if (!file) {
        std::cerr << "Failed to open file: " <<  filePath << std::endl;
		return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#include <dirent.h>

void HttpRequest::isDirectory(std::string& path)
{
    DIR* directory = opendir(path.c_str());
    if (directory != NULL) {
        closedir(directory);
			path = std::string("./site/default.html");
    }
}

/******************************************RESPONSES**********************************************/
/*************************************************************************************************/

template <class T>
std::string	numberToString(T nb)
{
	std::ostringstream	ss;
	ss << nb;
	return (ss.str());
}

// template <typename T>
// void printQueueElements(std::queue<T> q) {
//     while (!q.empty()) {
//         T element = q.front();
//         std::cout << MAGENTA << element << " " << DEFAULT;
//         q.pop();
//     }
//     std::cout << std::endl;
// }

void	HttpRequest::generateQueue(std::string response)
{
	std::size_t response_length = response.size();
	std::size_t chunk_size = 2048;
	std::size_t offset = 0;

	while (offset < response_length) {
	  // get current chunk
	  std::string chunk = response.substr(offset, chunk_size);

	  // add chunck to the queue
	  this->send_queue.push(chunk);

	  // move offset to next chunk
	  offset += chunk_size;
	}
}

void	HttpRequest::sendChunks(int fd)
{
	if (!(this->send_queue.empty()))
	{
    	std::string data = this->send_queue.front();
    	int bytes_sent = send(fd, data.c_str(), data.size(), 0);
    	if (bytes_sent == -1)
		{
    	    std::cerr << "Error : response bytes could not be sent (2)" << std::endl;
			this->_done = true;
			throw(Error("500"));
		}
		else if (bytes_sent == 0)
		{
			std::cerr << "Error " << RED<<"500"<<DEFAULT;
			std::cerr << " : nothing could be sent to client" << std::endl;
			throw(Error("500"));
		}
		else if (bytes_sent == static_cast<int>(data.size()))
		{
		    // current chunk completely sent
		    this->send_queue.pop();
		}
		else
		{
		    // remaining data is to be sent next time
		    std::string remaining_data = data.substr(bytes_sent);
		    this->send_queue.front() = remaining_data;
		}
		if (this->send_queue.empty())
			this->set_done(1);
	}
	else
		this->set_done(1);
}

void	HttpRequest::prepareResponse(std::string body, std::string status, std::string desc, std::string& response)
{
	response += _request["Version"];
	response += " ";
	response += status;
	response += " ";
	response += desc;
	response += "\r\n";
	char	time_buffer[1000];
	time_t	now = time(0);
	struct tm	tm = *gmtime(&now);
	strftime(time_buffer, 1000, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	response += "Date: ";
	response += time_buffer;
	response += "\r\nServer: Webserv\r\nContent-Length: ";
	response += numberToString(body.size());
	response += "\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
	response += body;
	std::cout <<GREEN<< response<<DEFAULT << std::endl;
}

void	HttpRequest::generateResponse(std::string body, const std::string &status, int fd)
{
	if (_ready == false)
	{
		std::string	response;
		std::map<std::string, std::string>	description;
		setDescription(description);

		if (status != "200" && status != "201")
		{
		    std::string error_file = "site/errors/" + status + ".html";
    	    std::ifstream file(error_file.c_str());

    	    if (file) {
    	        std::ostringstream oss;
    	        oss << file.rdbuf();
    	        body = oss.str();
    	    }
		}
		prepareResponse(body, status, description[status], response);
		generateQueue(response);
		_ready = true;
	}
	if (_ready == true)
		sendChunks(fd);
}
std::string combineDirectory(const std::string& directory1, const std::string& directory2) {
    // Check if the last character of directory1 is a '/'
    bool hasSlash1 = !directory1.empty() && directory1[directory1.size() - 1] == '/';

    // Check if the first character of directory2 is a '/'
    bool hasSlash2 = !directory2.empty() && directory2[0] == '/';

    // Decide whether to add a '/' between the two directories or not
    std::string separator = (hasSlash1 || hasSlash2) ? "" : "/";

    // Concatenate the two directories with the appropriate separator
    return directory1 + separator + directory2;
}

//? sendResponse
void	HttpRequest::sendResponse(const std::string &status, int fd)
{
	std::string	body;

	try {
		/**********************DELETE*********************/
		if (_request["Method"]== "DELETE")
		{
			delete_request();
			generateResponse(body, status, fd);
		}
		else if (_request["Method"] == "GET" && _request["URI"] == "/redirect")
			redir(fd);
		/********************DEFAULT GET*******************/
		else if (_request["Method"] == "GET" && status == "200")
		{
			if(_loc.content.empty())
			{
				std::cerr << "Error : loc = NULL" << std::endl;
				throw(Error("404"));
			}
			std::string	path_end = _request["URI"];
			path_end =	removeLeadingSlash(path_end);
			path_end = path_end.substr(_loc._path.length());
			path_end =	removeLeadingSlash(path_end);
			//std::cout<<RED<<_loc._path.length()<<"locpath:"<<_loc._path<<"uri:"<<_request["URI"]<<std::endl;
			if (path_end.empty())
				path_end = _loc._index;
			std::string	filePath = combineDirectory(_loc._root, path_end);
			isDirectory(filePath);

			std::ifstream	file(filePath.c_str());
			if (!file.is_open())
			{
				std::cerr << "Error : could not open file" <<  std::endl;
				throw(Error("404"));
			}
			std::getline(file, body, '\0');
			generateResponse(body, status, fd);
		}
	}


	catch (std::exception &e) {
		std::cerr << "Error : " <<RED<<e.what()<<DEFAULT<< " : delete or default get failed" <<  std::endl;
		DefaultError err;
		err.manageError(*this, e, fd);
		_done = 1;
	}
}

char** HttpRequest::create_combined_environment(char** env) {
    std::vector<std::string> environment;

    // Add existing environment variables to the vector
    if (env != NULL) {
        for (int i = 0; env[i] != NULL; ++i) {
            environment.push_back(env[i]);
        }
    }

    // Add CGI environment variables to the vector
    environment.push_back("REQUEST_METHOD=" + _request["Method"]);
    environment.push_back("REQUEST_URI=" + _request["URI"]);
    environment.push_back("SERVER_PROTOCOL=" + _request["Version"]);
    std::stringstream contentLengthStream;
	contentLengthStream << "CONTENT_LENGTH=" << _body.length();
	environment.push_back(contentLengthStream.str());
    environment.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");

    // Create the combined char** array
    char** combinedEnv = new char*[environment.size() + 1];
    for (size_t i = 0; i < environment.size(); ++i) {
        combinedEnv[i] = strdup(environment[i].c_str());
    }
    combinedEnv[environment.size()] = NULL;
    return combinedEnv;
}

std::string HttpRequest::get_file_suffix(const std::string& filename) {
    size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos && dotPos < filename.length() - 1) {
        return filename.substr(dotPos);
    }
    return "";
}

std::string HttpRequest::extractFilename(const std::string& requestBody)
{
    std::string filename;
    std::string filenamePrefix = "filename=\"";
    std::string filenameSuffix = "\"";

    size_t start = requestBody.find(filenamePrefix);
    if (start != std::string::npos) {
        start += filenamePrefix.length();
        size_t end = requestBody.find(filenameSuffix, start);
        if (end != std::string::npos) {
            filename = requestBody.substr(start, end - start);
        }
    }

    return (filename);
}

template<typename K, typename V>
void printMap(const std::map<K, V>& map) {
    typename std::map<K, V>::const_iterator it;
    for (it = map.begin(); it != map.end(); ++it) {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
    }
}

void HttpRequest::processCGI(const std::string& filenameSuffix) {
    std::string scriptPath;
    std::string root;
    // Recherche du CGI dans la location correspondant à _locPath
	if (_loc._cgi.count(filenameSuffix) > 0){
		scriptPath = _loc._cgi.at(filenameSuffix);
		root = _loc._root;	
	}
	else if (_config._cgi.count(filenameSuffix) > 0){
		scriptPath = _config._cgi.at(filenameSuffix);
		root = _config._root;
	}
    if (scriptPath.empty()) {
        std::cerr << "Error " << RED << "404" << DEFAULT;
        std::cerr << " : scriptPath empty in processCGI" << std::endl;
        throw Error("404");
    }
    _scriptPath = combineDirectory(root, scriptPath);
}

void	HttpRequest::receiveScriptResponse(std::string& response, int *pipe_receive_response)
{
	char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(pipe_receive_response[0], buffer, sizeof(buffer))) > 0)
	{
		if (bytesRead == 0)
		{
			std::cerr << "Error : " <<RED<<"500"<<DEFAULT << " : read" << std::endl;
			close(pipe_receive_response[0]);
			throw (Error("500"));
		}
        response.append(buffer, bytesRead);
    }
	close(pipe_receive_response[0]);
	if (bytesRead == -1)
	{
		std::cerr << "Error : " <<RED<<"500"<<DEFAULT << " : read" << std::endl;
		_done = 1;
		throw(Error("500"));
	}
}

void	HttpRequest::childPost(int *pipe_receive_response, int *pipe_send_body)
{
	close(pipe_send_body[1]);  //? Close the write end of the pipe
	close(pipe_receive_response[0]); // Close unused read end
    dup2(pipe_send_body[0], STDIN_FILENO);  //? Redirect stdin to the read end of the pipe
	close(pipe_send_body[0]);
	// Duplicate the write end of the receive_response pipe to stdout
    dup2(pipe_receive_response[1], STDOUT_FILENO);
    close(pipe_receive_response[1]);
	
    //? Execute the Python script
    char* argv[] = {const_cast<char*>(_scriptPath.c_str()), NULL};
    char* envp[] = {NULL};
    if (execve(_scriptPath.c_str(), argv, create_combined_environment(envp)) == -1)
	{
        perror("execve");
        exit(EXIT_FAILURE);
    }
}

void	HttpRequest::parentPost(int *pipe_receive_response, int *pipe_send_body, int pid)
{
	close(pipe_send_body[0]);  //? Close the read end of the pipe
	close(pipe_receive_response[1]); // Close unused write end

    //? Send the body data to the Python script through the pipe
    ssize_t bytesWritten = write(pipe_send_body[1], _body.c_str(), _body.length());
    if (bytesWritten == -1)
	{
		_done = 1;
		close(pipe_send_body[1]);  //? Close the read end of the pipe
		close(pipe_receive_response[0]); // Close unused write end
		std::cerr << "Error : " <<RED<<"500"<<DEFAULT;
		std::cerr << " : could not send data to script for post" << std::endl;
		throw(Error("500"));
	}
	close(pipe_send_body[1]);  //? Close the write end of the pipe

    //? Wait for the child process to finish
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        int exitStatus = WEXITSTATUS(status);
        std::cout << "Child process exited with status: " << exitStatus << std::endl;
    }
	else
        std::cerr << "Child process terminated abnormally." << std::endl;
}

void	HttpRequest::post_request(int fd, Config_List &serv_list)
{
	if (_ready == false)
	{
		
		int pipe_receive_response[2];
		int pipe_send_body[2];

		processCGI(get_file_suffix(_request["URI"]));
		(void)serv_list;

    	if (pipe(pipe_send_body) == -1 || pipe(pipe_receive_response) == -1) {
    	    perror("pipe");
    	    exit(EXIT_FAILURE);
    	}
    	pid_t pid = fork();
    	if (pid == 0)
    	    childPost(pipe_receive_response, pipe_send_body);
		else if (pid > 0)
		{
			parentPost(pipe_receive_response, pipe_send_body, pid);
			// Read the response from the script
    		std::string response;
    		receiveScriptResponse(response, pipe_receive_response);
	
			generateQueue(response);
			_ready = true;
			std::cout << GREEN << response << DEFAULT << std::endl;
		}
		else
		{
    	    perror("fork");
    	    exit(EXIT_FAILURE);
    	}
	}
	else if (_ready == true)
		sendChunks(fd);

}

void	HttpRequest::childDownload(int *pipe_receive_response, int *pipe_send_body, std::string scriptPath)
{
	// 1 for write 0 for read
    close(pipe_send_body[1]);
    close(pipe_receive_response[0]);

    dup2(pipe_send_body[0], STDIN_FILENO);
    close(pipe_send_body[0]);

    dup2(pipe_receive_response[1], STDOUT_FILENO);
    close(pipe_receive_response[1]);

    char* argv[] = {(char*)"python", (char*)_scriptPath.c_str(), NULL};
    char* envp[] = {NULL};
    execve(scriptPath.c_str(), argv, create_combined_environment(envp));
    perror("execve");
    exit(EXIT_FAILURE);
}

void	HttpRequest::parentDownload(int *pipe_receive_response, int *pipe_send_body, int pid)
{
    close(pipe_send_body[0]);
    close(pipe_receive_response[1]);

    // Send the body to the script
    ssize_t bytesWritten = write(pipe_send_body[1], _body.c_str(), _body.length());
    close(pipe_send_body[1]);
	if (bytesWritten == -1)
	{
		_done = 1;
		std::cerr << "Error : " <<RED<<"500"<<DEFAULT;
		std::cerr << " : could not send data to script for download" << std::endl;
		throw(Error("500"));
	}
	//? Wait for the child process to finish
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        int exitStatus = WEXITSTATUS(status);
        std::cout << "Child process exited with status: " << exitStatus << std::endl;
    }
	else
        std::cerr << "Child process terminated abnormally." << std::endl;
}

void HttpRequest::sendDownload(const std::string &status, int fd)
{	
	if (_ready == false)
	{
		(void)status;

    	std::string scriptPath = "./site/cgi-bin/cgi_script.py";
    	int pipe_send_body[2];
    	int pipe_receive_response[2];

    	if (pipe(pipe_send_body) == -1 || pipe(pipe_receive_response) == -1) {
    	    perror("pipe");
    	    exit(EXIT_FAILURE);
    	}

    	pid_t pid = fork();
    	if (pid == 0)
			childDownload(pipe_receive_response, pipe_send_body, scriptPath);
		else if (pid > 0)
		{
    	    parentDownload(pipe_receive_response, pipe_send_body, pid);
    	    std::string response;
    	    receiveScriptResponse(response, pipe_receive_response);
			std::cout << GREEN << response << DEFAULT << std::endl;

			generateQueue(response);
			_ready = true;
		}
		else
		{
    	    perror("fork");
    	    exit(EXIT_FAILURE);
    	}
    }
	else if (_ready == true)
		sendChunks(fd);
}

void	HttpRequest::delete_request(void)
{
	std::string filePath = "./site" + _request["URI"];
    int result = std::remove(filePath.c_str());

    if (result == 0)
        std::cout << "File has been removed successfully" << std::endl;
	else
	{
		std::cerr << "Error : " <<RED<<"404"<<DEFAULT;
        std::cerr << " : error during file suppression" << std::endl;
		throw (Error("404"));
    }
}

void	HttpRequest::redir(int fd)
{
	if (_ready == false)
	{
		std::string	response;

		response += "HTTP/1.1 301 Moved Permanently\r\n";
		response += "Location: http://localhost:";
		std::stringstream ss;
    	ss << _port;
    	std::string port = ss.str();

		response += port;
		response += "/redirection.html\r\n";
		response += "Content-Type: text/html\r\nContent-Length: 0\r\n";

		std::cout << GREEN << response << DEFAULT << std::endl;
		generateQueue(response);
		_ready = true;
	}
	else if (_ready == true)
		sendChunks(fd);
}

void	setDescription(std::map<std::string, std::string> &description)
{
	description["100"] = "Continue";
	description["101"] = "Switching protocols";
	description["200"] = "OK";
	description["201"] = "Created";
	description["202"] = "Accepted";
	description["203"] = "Non-Authoritative Information";
	description["204"] = "No content";
	description["205"] = "Reset Content";
	description["206"] = "Partial Content";
	description["300"] = "Multiple Choices";
	description["301"] = "Moved Permanently";
	description["302"] = "Found";
	description["303"] = "See Other";
	description["304"] = "Not Modified";
	description["305"] = "Use Proxy";
	description["307"] = "Temporary Redirect";
	description["400"] = "Bad request";
	description["401"] = "Unauthorized";
	description["402"] = "Payment Required";
	description["403"] = "Forbidden";
	description["404"] = "Not found";
	description["405"] = "Method not allowed";
	description["406"] = "Not Acceptable";
	description["407"] = "Proxy Authentication Required";
	description["408"] = "Request Timeout";
	description["409"] = "Conflict";
	description["410"] = "Gone";
	description["411"] = "Length required";
	description["412"] = "Precondition Failed";
	description["413"] = "Request entity too large";
	description["415"] = "Unsupported Media Type";
	description["416"] = "Requested Range Not Satisfiable";
	description["417"] = "Expectation Failed";
	description["422"] = "Unprocessable Entity";
	description["500"] = "Internal server error";
	description["501"] = "Not implemented";
	description["502"] = "Bad Gateway";
	description["503"] = "Service Unavailable";
	description["504"] = "Gateway Timeout";
	description["505"] = "HTTP Version not supported";
}
