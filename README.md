# 42Webserv

Webserv is one of the final projects of the 42 School common core. It consists of creating a server from scratch that handles basic HTTP requests (POST, GET, DELETE).
You can access to pages, try a redirection, upload files on the server and delete files present in the /site/uploads directory (ONLY files named delex1.txt, delex2.txt, delex3.txt)

# Launching and using the site

- Type "make"
- Use the webserv binary that has been created with a configuration file (its syntax is heavily inspired from nginx)
  ./webserv [configuration file]
- You are now able to access to the server by visiting localhost:[port] as the port number is set in the configuration file
  Different ports may lead to pages configurated differented, check the configuration file.
