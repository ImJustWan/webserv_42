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

	uint8_t	const &	getCgiStatus(void) const;
	void			setCgiStatus(uint8_t status);

	void			execCGI(void);
	void			sendResponse(void);

	class QuitProg : public std::exception {
		virtual const char* what() const throw() {return "";}
	};

private:
	CgiHandler();
	CgiHandler(CgiHandler const & src);
	CgiHandler & operator=(CgiHandler const & src);

	void		craftEnv(void);
	void		setFds(void);
	void		setArgv(void);
	void		accessChecks(std::string script, std::string interpreter);
	void		writePost(void);
	void		execParent(void);
	void		execChild(void);
	std::string	envContentLen(void);
	std::string	envContentType(void);
	void		childKiller(void);

	std::string	retrieveServerName(std::string request);
	std::string	retrieveScriptName(std::string request);
	void		retrievePathInfo(std::string resource);
	void		retrieveQueryString(std::string resource);
	void		checkCgiHeader(void);
	void		createCgiHeader(void);

	uint8_t			_CgiStatus; // Flag status : 1->for Post part 1; 2->for post part 2; or 3-> for GET; 4-> send response

	Request *		_theRequest;
	std::string		_path_info;		// fill with path of CGI script
	std::string		_script_name;	// extract script name 
	std::string		_query_string;	// if GET -> all after the ?
	std::string		_requestBody;

	int				_fds[2];
	int				_fdPost[2];
	pid_t			_pid;
	long long int	_time;
	std::map<std::string, std::string> _envMap;
	char			**_envp;
	char			**_argv;

	std::string		_response;
	std::string		_content_type;			//MIME type of the query data - look in _request for "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8"
	std::string		_content_length;		// for POST method : length of data passed to CGI through std input. 
	
	class ErrorInCGI : public std::exception {
	public:
		ErrorInCGI(const char * msg, int error) : _message(msg), _error(error) {}
		int getError(void) const {return this->_error;}
		virtual ~ErrorInCGI() throw() {};
		virtual const char* what() const throw() {return this->_message;}
	private:
		const char *	_message;
		int 			_error;
	};

};
