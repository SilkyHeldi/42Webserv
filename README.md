# 42Webserv

Webserv is one of the final projects of the 42 School common core. It consists of creating a server from scratch that handles basic HTTP requests following the RFC as much as possible (POST, GET, DELETE). Very specific instructions were given in the subject, free to you to discover/understand its subtilities.

# Launching

- Type "make"
- Use the webserv binary that has been created with a configuration file (its syntax is heavily inspired from nginx)
  ./webserv [configuration file]
- You are now able to access the server by visiting "localhost:[port]" in your searchbar as the port number is set in the configuration file
  Different ports may lead to pages configured differentely, check the configuration file you chose or created.

# Using the site

You can access pages, try a redirection, upload files on the server and delete files present in the /site/uploads directory (ONLY files named delex1.txt, delex2.txt, delex3.txt)
