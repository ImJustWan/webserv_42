// #ifndef __REQUEST__
// #define __REQUEST__

#pragma once

// # include "../inc/webserv.hpp"

# include <map>
# include <string>			/* Components for manipulating sequences of characters  */
# include <fcntl.h>			/* Manipulate file descriptor*/
# include <vector>
# include <iostream>		/* I/O streams library  */
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

class Request : IEvent {

public:

	Request( void );
	~Request();
	Request ( const Request& cpy );
	Request& operator= ( const Request& cpy );

	/* GETTERS */
	int					getEpfd() const;
	int					getEventSocket() const;
	std::string	const &	getRequest() const;
	std::string	const &	getMethod() const;

	/* SETTERS */
	void				setEpfd( const int epfd );
	void				setEventSocket( const int socket );
	void				setRequest( const int _baby_socket );

	/* METHODS */
	void				setAttributes();
	void				determinism();


protected:

	int					_epfd;
	int					_event_socket;
	std::string			_request;
	std::string			_method;
	std::string			_ressource;

	long				_valread;
};

// #endif /* __REQUEST__ */