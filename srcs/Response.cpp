#include "Response.hpp"
#include <filesystem>

/*****************  CANONICAL FORM *****************/

Response::Response() {
	_valread = 1;
	_status_code[400] = "Bad Request";
	_status_code[403] = "Forbidden";
	_status_code[404] = "Not found";
	_status_code[405] = "Method Not Allowed";
	_status_code[413] = "Payload Too Large";
	_status_code[415] = "Unsupported Media Type";
	_status_code[500] = "Internal Server Error";
	_status_code[501] = "Not Implemented";
	_status_code[504] = "Gateway Timeout";
	_status_code[505] = "HTTP Version Not Supported";
}

Response::~Response() {

}

Response::Response(Response const & src) : Request(src) {
	*this = src;
}

Response& Response::operator=(const Response& cpy) {
	if (this == &cpy)
		return (*this);
	Response::operator=(cpy);
	this->_header = cpy._header;
	this->_body = cpy._body;
	this->_status = cpy._status;
	return *this;
}


/*****************  CLASS METHODS *****************/


Location *Response::getLocation(void) const { return this->_location; }
Request	*Response::getCurrentRequest(void) const { return this->_currentRequest; }
void	Response::setCurrentRequest( Request *current ) { this->_currentRequest = current; }


void	Response::buildHeader( std::ifstream & file, unsigned int const & status_code )
{
	file.seekg(0, std::ios::end);
	int file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::ostringstream file_size_str;
	file_size_str << file_size;

	std::stringstream status_code_str;
	status_code_str << status_code;

	// Prepare HTTP response headers
	this->_header = "HTTP/1.1 ";
	this->_header += status_code_str.str();
	this->_header += " ";
	this->_header +=  _status_code[status_code];
	this->_header += "\r\n";
	this->_header += "Content-Length: ";
	this->_header += file_size_str.str();
	this->_header += "\r\n";
	this->_header += "Connection: Keep-Alive\r\n";
	this->_header += "\r\n";

	// this->_header += "Keep-Alive: timeout=5, max=1000" << "\r\n";

}

void	Response::setAttributes()
{
	std::istringstream	iss(this->_request);
	std::string			method;

	iss >> _method >> _resource; 
	// _resource.erase(0, 1);
}

bool	Response::extensionCheck()
{
	size_t extension = _resource.find_last_of('.');
	std::string ext;

	if (extension != std::string::npos) {
		ext = _resource.substr(extension);
		if (this->getCurrentServer()->getServerHandler()->getMimeMap().find(ext) == this->getCurrentServer()->getServerHandler()->getMimeMap().end())
			return ( false );
	}
	else if (extension == std::string::npos)
		return ( false );
	return ( true );
}

std::string	Response::trimSlash()
{
	std::string	buff;
	bool		endSlash = false;

	for (std::string::size_type i = 0; i < _resource.length(); i++) {
		if (_resource[i] == '/') {
			if (!endSlash)
				buff.push_back(_resource[i]);
			endSlash = true;
		}
		else {
			endSlash = false;
			buff.push_back(_resource[i]);
		}
	}
	return buff;
}

bool	Response::requestLineCheck( void )
{
	std::ifstream	file(this->_resource.c_str(), std::ios::binary);

	if (_resource.size() == 0)
		responseError(404);
	else if (access(_resource.c_str(), R_OK))
	{
		std::cout << "Can't access " << _resource << std::endl;
		responseError(403);
	}
	else if (!file.is_open())
		responseError(404);
	else if (_request.find("HTTP/1.1") == std::string::npos)
		responseError(505);
	else if (_resource.find("/..") != std::string::npos || _resource.find("../") != std::string::npos)
		responseError(403);
	// else if (!extensionCheck())
	// 	responseError(501);
	else
	{
		// std::cout << _EMERALD "We're good : " << _resource << _END << std::endl;
		return ( true );
	}
	// std::cout << _SALMON "We're NOT good : " << _resource << _END << std::endl;
	return ( false );
}

// Is there error_map and corresponding error
// Header for errors with macros fixed content of error_page

void	Response::errorPageBuilder(const uint16_t & status_code)
{
	std::stringstream status_code_str;
	status_code_str << status_code;

	this->_body += ERRPAGEBODY1;
	this->_body += status_code_str.str();
	this->_body += ERRPAGEBODY2;
	this->_body += status_code_str.str();
	this->_body += ERRPAGEBODY3;
	this->_body += _status_code[status_code];
	this->_body += ERRPAGEBODY4;

	this->_header += "HTTP/1.1 ";
	this->_header += status_code_str.str();
	this->_header += " ";
	this->_header +=  _status_code[status_code];
	this->_header += "\r\n";
	this->_header += "Content-Length: ";
	this->_header += this->_body.size();
	this->_header += "\r\n";
	this->_header += "Connection: Keep-Alive\r\n";
	this->_header += "\r\n";

	if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cout << _RED _BOLD "responseError: SEND HEADER" _END << std::endl;
	if (send(this->_event_socket,  this->_body.c_str(), this->_body.size(), 0) < 0)
		std::cout << _RED _BOLD "responseError: SEND BUFFER" _END << std::endl;
}


void	Response::responseError( const uint16_t & status_code )
{
	this->setValread(0);

	std::cout << _RED _BOLD "Response Error for : " << status_code <<  _END << std::endl;
	std::map<uint16_t, std::string>::const_iterator it = this->_current_server->getErrors().find(status_code);

	if (it != this->_current_server->getErrors().end())
	{
		char			buffer[4096];
		std::string		path = this->_current_server->getRoot() + it->second;
		std::ifstream	error_page(path.c_str(), std::ios::binary);
		if (!error_page){
			errorPageBuilder(status_code);
			return;
		}
		buildHeader(error_page, status_code);
		if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
			std::cout << _RED _BOLD "Error: SEND HEADER" _END << std::endl;
		while (!error_page.eof())
		{
			error_page.read(buffer, 4096);
			if (send(this->_event_socket, buffer, error_page.gcount(), 0) < 0) {
				std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
			}
		}
		error_page.close();
	}
	else
	{
		std::cout << _SALMON "NO MATCH, do your thing" _END << std::endl;
		errorPageBuilder(status_code);
	}
}

void	Response::executeMethod()
{
}

void	Response::sendResponse()
{
}
