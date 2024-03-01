#include "HttpRequest.hpp"

void DefaultError::manageError(HttpRequest &req, std::exception &e, int fd)
{
    std::map<std::string, std::string>	description;
    setDescription(description);
    Config tmp = req.getConfig();
    std::string root = tmp.getRoot(req.getLocation());
    std::string error = tmp.getErrorPage(req.getLocation(),e.what());
    //std::cout << MAGENTA <<"Root:\033[0m " << root << " | \033[1;31mError:\033[0m " << error << std::endl;
    std::string filename = root + error;

    std::string body = readFileIntoString(filename);
    int code = std::atoi(e.what());
    if (body.empty())
        body = readFileIntoString(errorMessages[code]);
    if (body.empty())
        throw(std::runtime_error("Error 500 : Failed to open file : " + errorMessages[code]));
    
    std::string	response;
    req.prepareResponse(body, e.what(), description[e.what()], response);
    int bytes_sent = send(fd, response.c_str(), response.size(), 0);
    if (bytes_sent == -1)
    {
        std::cerr << "Error : response bytes could not be sent (1)" << std::endl;
        req.set_done(1);
    }
}