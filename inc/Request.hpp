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

// # include "Server.hpp"
# define READ_STATE 0
# define WRITE_STATE 1

// # define GET	( 1 << 0 )
// # define POST	( 1 << 1 )
// # define DELETE	( 1 << 2 )
# define N_METHODS 3

# include "Server.hpp"

class	Response;
class	Server;

class Request : public IEvent {

public:

	Request( void );
	~Request();
	Request(const Request& src);
	Request& operator=(const Request& cpy);

	/* GETTERS */
	int const &				getEpfd() const;
	int const &				getValread() const;
	int const &				getEventSocket() const;
	std::string	const &		getRequest() const;
	std::string	const &		getResource() const;
	std::string	const &		getMethod() const;
	size_t					getContentLength(size_t const & found ) const;
	std::map<uint16_t, std::string>	const & 	getErrors(void) const;
	Server*					getCurrentServer() const;

	/* SETTERS */
	void					setEpfd( const int epfd );
	void					setEventSocket( const int socket );
	void					setValread( const int valread );
	void					setSocketState( bool state );
	void					setChunksEnd( bool state );
	void					setCurrentServer( Server *current );
	void					setResource( std::string resource );
	void					setRequest( std::string request );
	void					setRequest();
	void					setMethods();

	/* METHODS */

	void					setAttributes();
	void					determinism();
	void					buildResponse();
	void					initResponse( Response* response );
	Response*				newGet();
	Response*				newPost();
	Response*				newDelete();

	// void					writeFile();

	/* POST  */
	std::string	getKey(size_t & found);
	std::string	getValue(size_t & found, std::string const & queryKey);
	std::string	extractToken( char *searched, size_t & found );
	void		fillMap();
	std::string	getFilePath();

protected:

	bool				_socketState;
	Server*				_current_server;
	Response*			_response;
	Location*			_location;
	int					_epfd;
	int					_event_socket;
	std::string			_request;
	std::string			_method;
	std::string			_resource;
	int					_valread;
	bool				_finished;
	size_t				_readBytes;
	int					_methods;

};

typedef	struct	map_method {
	int			method;
	std::string	s_method;
	Response* (Request::*newMethod)();
}				map_method;
