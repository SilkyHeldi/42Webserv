/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llepiney <llepiney@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/10 06:18:27 by rgarrigo          #+#    #+#             */
/*   Updated: 2023/07/23 18:30:38 by llepiney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <algorithm>
# include <cstring>
# include <fstream>
# include <iostream>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>
# include <time.h>
# include <vector>
# include <iostream>
# include <unistd.h>
# include <sys/wait.h>
# include <exception>
# include <iostream>
# include <map>
# include <string>
# include <sys/types.h>
# include <cstring>
# include <cstdlib>
# include <fstream>
# include <streambuf>
# include <queue>
# include <fstream>
# include <sstream>
# include <ctime>
# include "../config/config.hpp"


# define CRLF "\r\n"
# define CRLF_DOUBLE "\r\n\r\n"
# define ENDCHUNK "0\r\n\r\n"

# define CONTENT_LENGTH "Content-Length"
# define TRANSFER_ENCODING "Transfer-Encoding"

# define BUFFER_SIZE 4096
# define TIMEOUT_DURATION 100

class DefaultError;

class HttpRequest
{
	private:
	// Utils
		std::time_t _currentTime;
		std::time_t _startTime;
		std::map<std::string, std::string>	_request;
		std::vector<std::string> _order;
		std::string	_body;
		std::string				_raw;
		bool	_reqlineComplete;
		bool	_headerComplete;
		bool	_bodyComplete;
		// bool					_headerComplete;
		// bool					_bodyComplete;
		bool					_ready;
		bool					_done;
		bool					_recv;
		int						_port;
		int						_fd;
		int						_count;
		bool					_chunked;
		bool					_oct_size;
		bool					_firstChunk;
		std::queue<std::string> send_queue;

		//config
		std::string					_scriptPath;
		std::string					_location;
		Config						_config;
		// Location					*_loc;

	// Methods utils
		void	_parseMethod(const std::string &method);
		void	_parseHeaderField(const std::string &field);
		void	_parseHeader(void);
		void	_parseBody(void);
		bool	_rawHeaderComplete(void) const;
		bool	_rawBodyComplete(void);
		void	_verifyHeader(void) const;
		void	lengthCheck(void);
		void	extensionCheck(void);
		void	manageChunkedBody(void);
		void	bodyFillCheck(int bytesRead, char *buffer, const Config_List &servlist);
		char** 	create_combined_environment(char** env);

	public:
		Location					_loc;
		//ADD TEST
		void	addVerifReqLine(std::string requestLine);
		void	reqline(std::string& _raw, size_t& crlfPos);
		void	headers(std::string& _raw, int bytesRead);
		void 	isDirectory(std::string& path);
		int 	checkMethodAllowed(const std::vector<std::string>& limit_except);
	// Coplien
		HttpRequest(void);
		HttpRequest(HttpRequest const &rhs);
		~HttpRequest(void);
		HttpRequest	&operator=(HttpRequest const &rhs);

	// Methods
		void			reqcv(int fd, const Config_List &servlist);
		bool			isComplete(void) const;
		void			generateQueue(std::string response);
		void			sendChunks(int fd);
		void			prepareResponse(std::string body, std::string status, std::string desc, std::string& response);
		void			generateResponse(std::string body, const std::string &status, int fd);
		void			sendResponse(const std::string &status, int fd);
		void			receiveScriptResponse(std::string& response, int *pipe_receive_response);
		void			childPost(int *pipe_receive_response, int *pipe_send_body);
		void			parentPost(int *pipe_receive_response, int *pipe_send_body, int pid);
		void 			post_request(int fd, Config_List &serv_list);
		void			childDownload(int *pipe_receive_response, int *pipe_send_body, std::string scriptPath);
		void			parentDownload(int *pipe_receive_response, int *pipe_send_body, int pid);
		void			sendDownload(const std::string &status, int fd);
		void 			delete_request(void);
		void			redir(int fd);
		int 			checkMethod();
		std::string		get_file_suffix(const std::string& filename);
		std::string		extractFilename(const std::string& requestBody);
		void			processCGI(const std::string& filenameSuffix);
		std::string 	readErrorPage(const std::string& filename);

	// Get set	
	// Getter for _scriptPath
		const std::string& getScriptPath() const;
		const std::string& getLocation() const;
		const std::string& getLocationPath() const;
		const Config& getConfig() const;
		std::string	get_method(void);
		std::string	get_uri(void);
		bool		get_done(void);
		void		set_done(int num);
		bool		get_recv(void);
		void		set_recv(int num);
		bool		get_ready(void);
		void		set_ready(int num);
		int			get_port(void);
		void		set_port(int num);
		int			get_fd(void);
		void		set_fd(int num);
		int			get_count(void);
		void		set_count(int num);
		void		set_currentTime(void);

		std::string			get_content_length(void);

	// Operators
		friend std::ostream	&operator<<(std::ostream &out, const HttpRequest &rhs);

	// Exceptions
		class Error;
};

class DefaultError {
private:
    std::map<int, std::string> errorMessages;

public:
    DefaultError() {
        // Initialisation des messages d'erreur HTTP
		errorMessages[100] = "site/errors/400.html";
		errorMessages[101] = "site/errors/400.html";
		errorMessages[200] = "site/errors/400.html";
		errorMessages[201] = "site/errors/201.html";
		errorMessages[202] = "site/errors/400.html";
		errorMessages[203] = "site/errors/400.html";
		errorMessages[204] = "site/errors/204.html";
		errorMessages[205] = "site/errors/400.html";
		errorMessages[206] = "site/errors/400.html";
		errorMessages[300] = "site/errors/400.html";
		errorMessages[301] = "site/errors/400.html";
		errorMessages[302] = "site/errors/400.html";
		errorMessages[303] = "site/errors/400.html";
		errorMessages[304] = "site/errors/400.html";
		errorMessages[305] = "site/errors/400.html";
		errorMessages[307] = "site/errors/400.html";
        errorMessages[400] = "site/errors/400.html";
        errorMessages[401] = "site/errors/400.html";
        errorMessages[402] = "site/errors/400.html";
        errorMessages[403] = "site/errors/403.html";
        errorMessages[404] = "site/errors/404.html";
        errorMessages[405] = "site/errors/405.html";
        errorMessages[406] = "site/errors/400.html";
        errorMessages[407] = "site/errors/400.html";
        errorMessages[408] = "site/errors/408.html";
        errorMessages[409] = "site/errors/400.html";
        errorMessages[410] = "site/errors/400.html";
        errorMessages[411] = "site/errors/411.html";
        errorMessages[412] = "site/errors/400.html";
        errorMessages[413] = "site/errors/413.html";
        errorMessages[414] = "site/errors/414.html";
        errorMessages[415] = "site/errors/415.html";
        errorMessages[416] = "site/errors/400.html";
        errorMessages[417] = "site/errors/400.html";
        errorMessages[422] = "site/errors/422.html";
        errorMessages[500] = "site/errors/500.html";
        errorMessages[501] = "site/errors/501.html";
        errorMessages[502] = "site/errors/400.html";
        errorMessages[503] = "site/errors/400.html";
        errorMessages[504] = "site/errors/400.html";
        errorMessages[505] = "site/errors/505.html";
    }

void manageError(HttpRequest &req, std::exception &e, int fd);
};


// Exceptions
class HttpRequest::Error : public std::exception
{
	private:
		std::string	_error;

	public:
    Error(const std::string &error) : _error(error) {}
    virtual const char *what(void) const throw() { return _error.c_str(); }
    virtual ~Error() throw() {}  // Remove the noexcept specifier
};


void				setDescription(std::map<std::string, std::string> &description);
std::string 		readFileIntoString(const std::string& filePath);
#endif
