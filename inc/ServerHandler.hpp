// #ifndef __SERVERHANDLER_HPP__
// #define __SERVERHANDLER_HPP__


#pragma once

# include "webserv.hpp"
# include "IServerHandler.hpp"

# define GET	( 1 << 0 )
# define POST	( 1 << 1 )
# define DELETE	( 1 << 2 )

class Server;

class	ServerHandler : public IServerHandler
{
public:

	ServerHandler();
	~ServerHandler();

	/*  Inherited methods */
	// void	createServers( std::map<std::string, int> map );
	void	createServers();
	void	handleServers();
	void	processHandler();
	void	addMasterSockets();
	// void	closeServers( Server& servers );


private:
	// uint16_t						port; //listen
	// uint8_t							allow_methods;
	// int								client_bodysize;
	std::vector<Server *>			_servers;
	int								_epfd;

	std::string						index; //index
	std::string						root; //root
	std::string						server_name; // server_name

	std::vector<std::string>		dirNames; // locations
	std::map<uint16_t, std::string>	errorcode, error_page; // list of error and their
};

// #endif
