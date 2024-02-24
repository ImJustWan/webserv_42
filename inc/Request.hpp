#pragma once

// # include "../inc/webserv.hpp"

# include <map>
# include <string>			/* Components for manipulating sequences of characters  */
# include <fcntl.h>			/* Manipulate file descriptor*/
# include <vector>
# include <iostream>		/* I/O streams library  */
# include <unistd.h>
# include <stdlib.h>
# include <exception>		/* Error handling library  */
# include <sys/stat.h>
# include <sys/epoll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <bits/stdc++.h>
# include "colors.hpp"
# include "IEvent.hpp"
# include "CgiHandler.hpp"
# include "Server.hpp"


// # include "Server.hpp"
# define READ_STATE 0
# define WRITE_STATE 1

// # define GET	( 1 << 0 )
// # define POST	( 1 << 1 )
// # define DELETE	( 1 << 2 )
# define N_METHODS 3
# define TIMEOUT 10



class	Response;
class	Server;
class	ServerHandler;
class	CgiHandler;

class Request : public IEvent {

public:

	Request( void );
	~Request();
	Request(const Request& src);
	Request& operator=(const Request& cpy);

	/* GETTERS */
	ServerHandler*			getServerHandler() const;
	Server*					getCurrentServer() const;
	Location*				getLocation() const;
	Response*				getResponse() const;
	int const &				getEpfd() const;
	int const &				getReadBytes() const;
	int const &				getEventSocket() const;
	int const &				getMethods() const;
	int const &				getListen() const;
	std::string	const &		getRequest() const;
	std::string	const &		getIndex() const;
	std::string	const &		getHost() const;
	std::string	const &		getRoot() const;
	std::string	const &		getResource() const;
	std::string	const &		getMethod() const;
	std::string	const &		getHTTP() const;
	bool					getReady() const;
	bool					getReadFinished() const;
	bool					getSentFinished() const;

	/* SETTERS */
	void					setServerHandler( ServerHandler *serverHandler );
	void					setCurrentServer( Server *current );
	void					setLocation( Location *location );
	void					setResponse( Response *response );
	void					setEpfd( const int epfd );
	void					setEventSocket( const int socket );
	void					setSocketState( bool state );
	void					setIndex( std::string index );
	void					setRoot( std::string root );
	void					setHost( std::string host ); 
	void					setRequest( std::string request );
	void					setResource( std::string resource );
	void					setListen( const int listen );
	void					setMethods( const int methods );
	void					setReadBytes( const int readbytes );
	void					setLastEvent( void );
	void					setLastEvent(long long int time);
	void					setAsReady(bool state);
	void					setReadFinished(bool state);
	void					setSentFinished(bool state);

	/* METHODS */

	void					determinism();
	bool					checkTimeout();

	void					buildResponse();
	void					buildResponse( const uint16_t & status_code );


protected:

	ServerHandler*			_currentServerHandler;
	Server*					_currentServer;
	Location*				_currentLocation;
	Response*				_currentResponse;
	CgiHandler*				_currentCGI;

	int						_epfd;
	int						_event_socket;
	bool					_socketState;

	std::string				_index;
	std::string				_root;
	std::string				_host;
	std::string				_request;
	std::string				_method;
	std::string				_http;
	std::string				_resource;
	std::string				_cgiExt;

	int						_listen;
	int						_methods;

	int						_readBytes;
	bool					_readFinished;
	bool					_sentFinished;
	bool					_responseReady;
	long long int			_readLength;
	long long int			_contentLength;
	long long int			_lastEvent;

private :

	void					setRequest();
	void					setAttributes();
	void					setLocation();
	void					setMethodsRootIndex();
	void					changeSocketState();

	bool					isCGI(std::string const & resource);
	size_t					findContentLength(size_t const & found ) const;

	Response*				newGet();
	Response*				newPost();
	Response*				newDelete();

};

typedef	struct	map_method {
	int			method;
	std::string	s_method;
	Response* (Request::*newMethod)();
}				map_method;
