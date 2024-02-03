#include "webserv.hpp"

Server::Server() {
	std::cout << "Default constructor" << std::endl;
	initData();
}

Server::~Server() {
	close(this->_master_socket);
}

void	Server::initData()
{
	this->_baby_index = 0;
	this->_port = 8081; // default, to be changed
	this->ling.l_onoff = 1;
	this->ling.l_linger = 0;
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons( this->_port );
	addrlen = sizeof(address);
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));
	memset(client_socket, 0, sizeof(client_socket));
}

Server::Server ( const Server& cpy ) : IEvent(cpy) {
	*this = cpy;
}

Server& Server::operator= ( const Server& cpy ) {
	(void)cpy;
	// this->_port = cpy._port;
	// this->ling.l_onoff = cpy.ling.l_onoff;
	// this->ling.l_linger = cpy.ling.l_linger;
	// this->address.sin_family = cpy.address.sin_family;
	// this->address.sin_addr.s_addr = cpy.address.sin_addr.s_addr;
	// this->address.sin_port = cpy.address.sin_port;
	// addrlen = sizeof(address);
	// memset(address.sin_zero, '\0', sizeof(address.sin_zero));
	// memset(client_socket, 0, sizeof(client_socket));
	return *this;
}


/* ****************  GETTERS **************** */

int	Server::getEpfd() const {
	return (this->_epfd);
}

int	Server::getMasterSocket() const{
	return (this->_master_socket);
}

int	Server::getBabySocket(int index) const {
	return (this->_baby_socket[index]);
}


/* ****************  SETTERS **************** */

void	Server::setEpfd(const int epfd) {
	this->_epfd = epfd;
}


/* ****************  CLASS METHODS **************** */

void	Server::determinism()
{
	std::cout << _AQUAMARINE "Determining Server " _END << std::endl;
	serverProcess();
}

void	Server::buildSocket( void ) {
	if ((this->_master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw SocketError();
	if (setsockopt(this->_master_socket, SOL_SOCKET, SO_REUSEADDR, &ling, sizeof(ling)) < 0)
		throw SocketOptionError();
	if (bind(this->_master_socket, (struct sockaddr *)&address, sizeof(address))<0)
		throw BindError();
	if (listen(this->_master_socket, 10) < 0)
		throw ListenError();
}

void Server::serverProcess() {
	struct epoll_event interestList;

	interestList.events = EPOLLIN;

	if ((this->_baby_socket[_baby_index] = accept(this->_master_socket, (struct sockaddr *)&address, &addrlen)) < 0)
		throw SocketLoopError();

	Request *current = new Request;

	current->setEventSocket(this->_baby_socket[_baby_index]);
	current->setEpfd(this->_epfd);
	interestList.data.ptr = current;
	if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, this->_baby_socket[_baby_index], &interestList) == -1)
		std::cout << "MASTER epoll error" << std::endl;
	_baby_index++;
}

