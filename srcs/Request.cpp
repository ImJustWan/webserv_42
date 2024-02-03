# include "Request.hpp"
# include "Response.hpp"

Request::Request() {

}

Request::~Request() {

}

Request::Request ( const Request& cpy ) : IEvent(cpy) {
	*this = cpy;
}

Request& Request::operator= ( const Request& cpy ) {
	(void)cpy;
	return *this;
}


/* ****************  GETTERS **************** */

int	Request::getEpfd() const {
	return (this->_epfd);
}

int	Request::getEventSocket() const {
	return (this->_event_socket);
}

std::string	const & Request::getRequest() const {
	return (this->_request);
}

std::string	const & Request::getMethod() const {
	return (this->_method);
}


/* ****************  SETTERS **************** */

void	Request::setEpfd( const int epfd ) {
	this->_epfd = epfd;
}

void	Request::setEventSocket( const int socket ) {
	this->_event_socket = socket;
}

void	Request::setRequest() {
	char	buffer[4096];

	_valread = read(this->_event_socket, buffer, sizeof(buffer) - 1);
	buffer[_valread] = 0;
	_request = buffer;
}


/* ****************  CLASS METHODS **************** */


void				Request::setAttributes()
{
	std::istringstream	iss(Request::_request);
	std::string			method;

	iss >> _method >> _ressource; 
	_ressource.erase(0, 1);
}

void	Request::determinism()
{
	std::cout << _SALMON "Determining Request " _END << std::endl;
	Response response;
	response.setEventSocket(this->_event_socket);
	response.craftResponse();
	response.sendResponse();
}