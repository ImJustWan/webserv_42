// #ifndef __RESPONSE__
// #define __RESPONSE__

#pragma once

# include "Request.hpp"

struct header {
	int				status_code; // (?)
	std::string		date;
	std::string		server;
	std::string		content_type;
	std::string		content_length;
	bool			connection; // keep-alive/closed
	// std::string location;
};

class Response : public Request
{

public:

	Response();
	~Response();
	Response ( const Response& cpy );
	Response& operator= ( const Response& cpy );

	void		sendResponse( int const &  _baby_socket);
	void		sendFile(const std::string & file_path, int const & _baby_socket );
	void		buildHeader( std::ifstream & file, std::ostringstream & headers );

private:

	// uint16_t		_status;
	// int				_port;
	// int				_socket_fd;

	// std::string		_root;
	// bool			_cgi;
	// bool			_auto_index;
	// std::string		_body; // repsonse
	// struct header	_headers;


	// void		readResource( ??? );
	// void		buildResource( ??? );
	// void		buildHeader( ??? );
	// void		buildResponse( ??? );
	// void		sendResponse( ??? );
};

// #endif /* __RESPONSE__ */