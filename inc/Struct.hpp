
#ifndef STRUCT_HPP
# define STRUCT_HPP

struct DefaultListenServer
{
	Server *_server;
	Listen _listen;
	DefaultListenServer(void) {}
	DefaultListenServer(Server *server, Listen &listen) : _server(server), _listen(listen) {}
};

#endif
