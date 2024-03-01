#ifndef CONFIG_HPP
# define CONFIG_HPP
 
# include <iostream>
# include <fstream>
# include <string>
# include <streambuf>
# include <sstream>
# include <vector>
# include <stack>
# include <iomanip>
# include "../utils/colors.hpp"
# include <string>
# include <algorithm>
# include <map>
# include <set>
# define BODY_LIMIT	(6 * 1024 * 1024)

class Location {
public:
	Location() : content(""), _path(""), _root(""), _auth(""), _index(""),_client_max_body_size(""), _cgi() {}
	Location& operator=(const Location& other);
	Location(const Location& other);

    std::string 							content;
	std::string								_path;
	std::string								_root;
	std::string 							_auth;
	std::string 							_index;
	std::string								_client_max_body_size;
	std::vector<std::string> 				_limit_except;
	std::map<std::string, std::string>		_error_page;
	std::map<std::string, std::string>		_cgi;

	void display() const;
};

class Config {
public:
	Config(std::string conf);
	Config() : _port("0"), _root(""), _client_max_body_size("") {}
	Config& operator=(const Config& other);
	Config(const Config& other);

	std::vector<std::string>				_servername;
	std::vector<Location>					_locations;
	std::string		 						_port;
	std::map<std::string, std::string>		_cgi;
	std::map<std::string, std::string>		_error_page;
	std::string								_root;
	std::string								_client_max_body_size;
	std::vector<std::string> 				_limit_except;
	void display() const;
	Location*  ReturnLocation(const std::string& _uri) const;
	std::string getRoot(const std::string& locPath) const;
	std::string getBodyLimitForLocation(const std::string& locPath) const;
	std::string getErrorPage(const std::string& locPath, const std::string& error) const;
	Location	findMatchingLocation(const std::string& uri);
};

class Config_List {
public:
	Config_List(std::string conf);
	

	std::vector<Config> _ConfigList;
	void display() const;

	int checkForPortAndServerNameConflict() const;
	std::vector<std::string> getDistinctPorts() const;
	std::string getLocationRoot(std::string& locationPath, std::string port);
	Config getConfigByPortAndServerName(const std::string& port, const std::map<std::string, std::string>& hostMap) const;
};

class ConfigListException : public std::exception {
public:
    ConfigListException(const char* message) : msg(message) {}
    virtual const char* what() const throw() {
        return msg;
    }
private:
    const char* msg;
};

std::string 		removeLeadingSlash(const std::string& str);
void 				ft_trim(std::string& str);
void 				removeTrailingSemicolon(std::string& str);
template<typename K, typename V>
void				printMap(const std::map<K, V>& map);
std::string 		removeLeadingSlash(const std::string& str);

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define DEFAULT "\033[39m"

#endif
