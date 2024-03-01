#include "config/config.hpp"
#include "server/server.hpp"

int main(int argc, char **argv) {
    if (argc > 2)
    {
        std::cerr << "Error : wrong number of arguments" << std::endl;
        return (0);
    }
    try
    {
        if (argc == 1)
        {
            Server  server;
            try{
            Config_List servlist("default.conf");
            // servlist.display();
            server.start_server(servlist);}
            catch (const ConfigListException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;}
        }
        else
        {
            Server  server;
            try{
            Config_List servlist(argv[1]);
            // servlist.display();
            server.start_server(servlist);}
            catch (const ConfigListException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;}
        }
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
    }