// #ifndef __SERVER_HPP__
// #define __SERVER_HPP__

#pragma once

# define MAX_CLIENTS 30
# define MAX_BBY_SOCKET 3

# include "webserv.hpp"

class Server : public IEvent
{

public:
	Server();
	~Server();
	Server(Server const &src);
	Server & operator=(Server const & src);


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

	/* GETTERS */
	int		getEpfd() const;
	int		getMasterSocket() const;
	int		getBabySocket(int index) const;

	/* SETTERS */
	void	setEpfd(const int epfd);

	/* METHODS */
	void	buildSocket( void );
	void	initData();
	void	determinism();
	void	serverProcess();
	std::map<int, bool> socketReadStates; 

private:


	int							_epfd;
	int							_port;
	std::string					_server_name;
	int							_master_socket;
	int							_baby_index;
	int							_baby_socket[MAX_BBY_SOCKET];
	int							client_socket[MAX_CLIENTS];
	socklen_t					addrlen;
	struct sockaddr_in			address;
	struct linger				ling;
	std::string					message;
	// long						valread;
	// uint8_t					_allowed_methods;
};


// #endif /* __SERVER__ */
