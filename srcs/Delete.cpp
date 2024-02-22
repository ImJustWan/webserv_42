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
	this->_response = this->_header + this->_body;
	this->_header += this->_body;

	if (send(this->getCurrentRequest()->getEventSocket(), this->_response.c_str(), this->_response.size(), 0) < 0)
		this->getCurrentRequest()->setLastEvent(0);
}

void	Delete::deleteResource()
{
	if (remove(this->getCurrentRequest()->getResource().c_str()) == 0)
	{
		std::cout << _EMERALD "Successfully deleted " << this->getCurrentRequest()->getResource() << _END << std::endl;
		this->_status = "202 Accepted";
		this->_body = "Deletion request accepted and will be processed";
		sendResponse();
	}
	else
	{
		std::cout << _RED "Failure to delete " << this->getCurrentRequest()->getResource() << _END << std::endl;
		responseError(404);
	}
	this->getCurrentRequest()->setReadBytes(0);
}

void	Delete::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Delete" _END << std::endl;
	std::string path = this->trimSlash();
	this->getCurrentRequest()->setResource(this->getCurrentRequest()->getRoot() + path);
	if (requestLineCheck())
		this->deleteResource();
}

