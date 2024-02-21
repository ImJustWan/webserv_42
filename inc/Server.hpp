#pragma once

# include "webserv.hpp"
# include "Location.hpp"
# include "Request.hpp"

# define MAX_CLIENTS 30
# define MAX_BBY_SOCKET 1000
# define MAX_REQUEST 1000

# define INVALCONF "Invalid Configuration: "

# define GET	1
# define POST	2
# define DELETE	4

class Request;
class Location;
class ServerHandler;

class Server : public IEvent {

public:

	Server(std::vector<std::string> &tokens, int id);
	~Server();

/* GETTERS */
	int											getServerID(void) const;
	uint16_t const & 							getListen(void) const;
	int const & 								getMethods(void) const;
	std::string const & 						getRoot(void) const;
	std::string const & 						getIndex(void) const;
	bool const & 								getAutoindex(void) const;
	long long int const & 						getClientMaxBodySize(void) const;
	std::vector<std::string> const & 			getServerNames(void) const;
	std::map<uint16_t, std::string>	const & 	getErrors(void) const;
	std::map<std::string, Location *> const & 	getLocations(void) const;

	int const & 								getEpfd(void) const;
	int const & 								getReqIndex(void) const;
	int const & 								getMasterSocket(void) const;
	int const & 								getBabySocket(size_t idx) const;
	ServerHandler *								getServerHandler(void) const;
	Request *									getRequest(int index);

	/* SETTERS */
	void	setListen(std::vector<std::string>);
	void	setMethods(std::vector<std::string>);
	void	setRoot(std::vector<std::string>);
	void	setIndex(std::vector<std::string>);
	void	setAlias(std::vector<std::string>);
	void	setAutoindex(std::vector<std::string>);
	void	setClientMaxBodySize(std::vector<std::string>);
	void	setServerNames(std::vector<std::string>);
	void	setLocations(std::vector<std::string>);
	void	setErrors(std::vector<std::string>);

	void	setEpfd(const int epfd);
	void	setMasterSocket(int masterSocket);
	void	setBabySocket(int * sockets);
	void	setBabySocket(int idx, int socket);
	void	setServerHandler(ServerHandler *serverHandler);
	void 	eraseRequest(int index);

	/* METHODS */
	void	buildSocket( void );
	void	determinism();
	

	/* EXCEPTIONS */
	class InvalidConfig : public std::exception {
	public:
		InvalidConfig(const char * msg) : _message(msg) {}
		virtual ~InvalidConfig() throw() {};
		virtual const char* what() const throw() {return this->_message;}
	private:
		const char * _message;
	};

	class SocketError : public std::exception {
		public: virtual const char * what() const throw() {return "In Socket";}
	};

	class SocketOptionError : public std::exception {
		public: virtual const char * what() const throw() {return "In Socket option";}
	};

	class SocketLoopError : public std::exception {
		public: virtual const char * what() const throw() {return "In Socket loop";}
	};

	class BindError : public std::exception {
		public: virtual const char * what() const throw() {return "In Bind";}
	};

	class ListenError : public std::exception {
		public: virtual const char * what() const throw() {return "In Listen";}
	};

private:

	Server();

	Server(const Server& src);
	Server& operator=(const Server& cpy);

	void configurationMap(void);
	void directiveIsolation(std::string delim, std::vector<std::string> &tokens,
		std::map<std::string, void (Server::*)(std::vector<std::string>)>::iterator configIt,
		std::vector<std::string>::iterator i);
	void checkMinimumConf(void);
	void earlyDeath(void);
	void checkForWhitelist(std::vector<std::string> &directive);

	void	serverProcess();
	void	initRequest( Request* request );

	template <typename T>
	T dataExtractor(std::string const &src)
	{
		std::stringstream	ss(src);
		T value;
		ss >> value;
		return value;
	}


	unsigned  int						_serverID;
	uint16_t							_listen;
	std::string							_root;
	std::string							_index;
	bool								_autoindex;
	int									_methods;
	long long int						_clientMaxBodySize;
	std::vector<std::string>			_serverNames;
	std::map<std::string, Location *>	_locations;
	std::map<uint16_t, std::string>		_errors;

	std::map<std::string, void(Server::*)(const std::vector<std::string>)> _configMap;

	ServerHandler				*_serverHandler;
	int							_epfd;
	int							_port;
	int							_master_socket;
	int							_request_index;
	int							_baby_socket[MAX_BBY_SOCKET];
	int							client_socket[MAX_CLIENTS];
	socklen_t					addrlen;
	struct sockaddr_in			address;
	struct linger				ling;
	std::string					message;
	long long int				_timeout;
	Request*					_requests[MAX_REQUEST];

};


inline std::ostream & operator<<(std::ostream & o, Server const & rhs)
{
	std::vector<std::string>	serverNames = rhs.getServerNames();
	std::map<uint16_t, std::string>	errors = rhs.getErrors();
	std::map<std::string, Location *>	locations = rhs.getLocations();
	o << "+··················································+\n";
	o << _BOLD _CYAN "\t\t\tSERVER: " << rhs.getServerID() << "\n" _END;
	o << "+··················································+\n";
	o << _BOLD " · Listen\t\t\t" _END << rhs.getListen() << "\n";
	o << _BOLD " · Root\t\t\t\t" _END << rhs.getRoot() << "\n";
	o << _BOLD " · Index\t\t\t" _END << rhs.getIndex() << "\n";
	o << _BOLD " · Autoindex\t\t\t" _END << (rhs.getAutoindex()?"true":"false") << "\n";
	o << _BOLD " · Methods\t\t\t" _END << rhs.getMethods() << "\n";
	o << _BOLD " · CMBS\t\t\t\t" _END << rhs.getClientMaxBodySize() << "\n";
	o << _BOLD "\n · Server names" _END " · · · · · · · · · · · · · · · · · ·+\n";
	for (std::vector<std::string>::iterator i = serverNames.begin(); i != serverNames.end(); ++i)
		o << "\t\t\t\t" << *i << "\n";
	o << _BOLD "\n · Error Pages" _END "  · · · · · · · · · · · · · · · · · ·+\n" ;
	for (std::map<uint16_t, std::string>::iterator i = errors.begin(); i != errors.end(); ++i)
		o << "\t\t\t" << i->first << "\t" << i->second << "\n";
	o << std::endl;
	for (std::map<std::string, Location *>::iterator i = locations.begin(); i != locations.end(); ++i) {
		o << "----------------------------------------------------\n";
		o << *(i->second);
	}
	o << "+··················································+\n" << std::endl;
	return o;
};
