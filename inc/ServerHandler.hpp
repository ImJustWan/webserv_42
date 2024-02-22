#pragma once

# include "webserv.hpp"
# include "Server.hpp"
# include "colors.hpp"

# define DEFAULT_CONF "./config/default.conf"
# define DELIMITERS "{};"
# define WHITESPACE " \t"

class Server;

class	ServerHandler {
public:

	ServerHandler();
	ServerHandler(std::string const &filename);
	~ServerHandler();

	void	handleServers();
	void	addMasterSockets();

	std::map<std::string, std::string> const & getMimeMap(void) const;
	std::vector<Server *> const & getServers(void)const;		// returns the vector containing servers;
	Server * operator[](size_t id); 							// returns a reference to the requested server;

	class InvalidConfFileException : public std::exception {
	public:
		virtual const char* what() const throw() {return _RED "Invalid File" _END;}
	};

	class InvalidIndexException : public std::exception {
	public:
		virtual const char* what() const throw() {return _RED "Invalid index requested in operator []" _END;}
	};

	class InvalidDirectiveException : public std::exception {
	public:
		virtual const char* what() const throw() {return _RED "Invalid Directive" _END;}
	};

	class InvalidBlockException : public std::exception {
	public:
		virtual const char* what() const throw() {return _RED "Invalid Block count" _END;}
	};

	class EpollException : public std::exception {
	public:
		virtual const char* what() const throw() {return _RED "Epoll create Error" _END;}
	};

private:
	struct epoll_event							_readyList[30];
	std::vector<Server *>						_servers;
	int											_epfd;

	std::vector<std::string>					_tokens;
	std::map<std::string, std::string>			_mimeMap;

	void constructorExtension(std::string const & filename);	// factorisation of the steps to build a ServerHandler Object with and without parameter.
	void lexer(std::string const &file);						// converts std::string to vector<std::string> --> function called by constructor;
	void fileChecker(std::string const &file);					// Checks if configuration file can be accessed and is not empty;
	void checkBrackets(std::vector<std::string> &rawData);		// Checks if the number of brackets is correct - ensures that each block is properly closed;
	std::vector<std::vector<std::string > > ServerInfo(std::vector<std::string> &rawData);
																// Counts the number of servers and sort infos in a vector of vector of string;
	void checkForWhitelist(void);								// Check for whitelisted directives and for directives format.

	ServerHandler(ServerHandler const &src);
	ServerHandler & operator=(ServerHandler const &src);
	void		createMimeMap();		// creates a map of mime types;

};
