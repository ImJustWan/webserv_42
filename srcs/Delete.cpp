#include "Delete.hpp"

/*****************  CANONICAL FORM *****************/

Delete::Delete() {
}

Delete::~Delete() {
}

Delete::Delete(Delete const & src) : Response(src) {
	*this = src;
}

Delete& Delete::operator=(const Delete& cpy) {
	if (this == &cpy)
		return (*this);
	Response::operator=(cpy);
	return *this;
}


/*****************  CLASS METHODS *****************/

void	Delete::buildHeader()
{
	this->_header += "HTTP/1.1 ";
	this->_header += this->_status;
	this->_header += "\r\n";
	this->_header += "Content-Type: text/plain\r\n";
	this->_header += "Connection: Keep-Alive\r\n";
	this->_header += "\r\n";
}

void	Delete::sendResponse()
{
	this->buildHeader();
	this->_header += this->_body;
	
	if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cout << _RED _BOLD "Error: SEND HEADER" _END << std::endl;
	// if (send(this->_event_socket,  this->_body.c_str(), this->_body.size(), 0) < 0)
	// 	std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
	// std::cout << _PINK "RESPONSE\n" << _header << _body << _END << std::endl;
}

void	Delete::deleteResource()
{
	if (remove(this->_resource.c_str()) == 0)
	{
		std::cout << _EMERALD "Successfully deleted " << this->getResource() << _END << std::endl;
		this->_status = "202 Accepted";
		this->_body = "Deletion request accepted and will be processed";
		sendResponse();
	}
	else
	{
		std::cout << _RED "Failure to delete " << this->getResource() << _END << std::endl;
		responseError(404);
	}
	_valread = 0;
}

void	Delete::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Delete" _END << std::endl;
	std::string path = this->trimSlash();
	_resource = _root + path;
	std::cout << _EMERALD "Resource is : " << _resource << _END << std::endl;
	if (requestLineCheck())
		this->deleteResource();
}

