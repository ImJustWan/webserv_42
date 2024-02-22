# pragma once

# include <iostream>
# include <stdbool.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <unistd.h>
# include <fstream>
# include <cstdlib>
# include <sys/types.h>
# include <sys/socket.h>

# include "colors.hpp"
# include "Request.hpp"

# define READ 0
# define WRITE 1

class Request;

class CgiHandler {

public:
	CgiHandler(Request * theRequest);
	~CgiHandler();

private:
	CgiHandler();
	CgiHandler(CgiHandler const & src);
	CgiHandler & operator=(CgiHandler const & src);

	void	craftEnv(void);
	std::string retrieveServerName(std::string request);
	std::string retrieveScriptName(std::string request);

	std::string		_gateway_interface;		// hardcode it! "Our_CGI/1.0"
	std::string		_server_name;			// look in _request for IP address or Hostname "Referer: http://127.0.0.1:8080/"
	std::string		_server_software;		// WebSlayyy/1.0
	std::string		_server_protocol;		// HTTP/1.1
	std::string		_server_port;			// retrieve from request::_currentServer;
	std::string		_request_method;		// retrieve from Request::_method;
	std::string		_path_info;				// fill with path of CGI script
	std::string		_path_translated;		// fill with path of CGI script
	std::string		_script_name;			// extract script name 
	std::string		_document_root;			// to check later ? www or cgi-bin ? 
	std::string		_query_string;			// if GET -> all after the ?
	Request *		_theRequest;
	// std::string		_remote_host;
	// std::string		_remote_addr;
	// std::string		_auth_type;
	// std::string		_remote_user;
	// std::string		_remote_ident;
	std::string		_content_type;			//MIME type of the query data - look in _request for "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8"
	std::string		_content_length;		// for POST method : length of data passed to CGI through std input. 
	// std::string		_http_from;
	// std::string		_http_accept;
	// std::string		_http_user_agent;
	// std::string		_http_referer;
	int				_pid;
	int				_fdGet[2];
	int				_fdPost[2];
	std::string		_currentResponse;
	std::map<std::string, std::string> _envMap;

};
