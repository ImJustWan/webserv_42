#include "Response.hpp"

Response::Response() {
	_valread = 1;
	_status_code[400] = "Bad Request";
	_status_code[403] = "Forbidden";
	_status_code[404] = "Not found";
	_status_code[405] = "Method Not Allowed";
	_status_code[413] = "Payload Too Large";
	_status_code[500] = "Internal Server Error";
	_status_code[501] = "Not Implemented";
	_status_code[504] = "Gateway Timeout";
	_status_code[505] = "HTTP Version Not Supported";
}

Response::~Response() {

}

Response::Response ( const Response& cpy ) : Request(cpy) {
	*this = cpy;
}

Response& Response::operator= ( const Response& cpy ) {
	(void)cpy;
	return *this;
}

void	Response::buildHeader( std::ifstream & file, unsigned int const & status_code )
{
	file.seekg(0, std::ios::end);
	int file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::ostringstream file_size_str;
	file_size_str << file_size;

	// Prepare HTTP response headers
	this->_header += "HTTP/1.1 ";
	this->_header += status_code;
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
	_resource.erase(0, 1);
}

// Is there matching location
// allowed methods on said location
// is the resource accessible

bool	Response::requestLineCheck( void )
{
	std::ifstream	file(this->_resource.c_str(), std::ios::binary);

	if (_resource.size() == 0 || !file.is_open())
		responseError(404);
	else if ( access(_resource.c_str(), R_OK) )
		responseError(403);
	else if (_request.find("HTTP/1.1") == std::string::npos)
		responseError(505);
	else
		return ( true );
	return ( false );
}

// Is there error_map and corresponding error


void	Response::responseError( const unsigned int & status_code )
{
	this->setValread(0);
	std::map<uint16_t, std::string>::const_iterator it = this->_current_server->getErrors().find(status_code);
	if (it != this->_current_server->getErrors().end())
	{
		std::cout << _EMERALD "FOUND A MATCH, can we use it ?" _END << std::endl;

		std::cout << "Error : " << it->second << std::endl;

	}
	else
	{

		std::cout << _SALMON "NO MATCH, do your thing" _END << std::endl;
	}

	std::stringstream status_code_str;
	status_code_str << status_code;
	_header += "HTTP/1.1 ";
	_header += status_code_str.str();
	_header += " ";
	_header += _status_code[status_code];
	_header += "\r\n";
	_header += "Content-Type: text/plain\r\n";
	_header += "Content-Length: 60";
	_header += "\r\n";
	_header += "Connection: Keep-Alive\r\n";
	_header += "\r\n";
	this->_body = "Resource not found. The requested resource does not exist.";
	if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cout << _RED _BOLD "responseError: SEND HEADER" _END << std::endl;
	if (send(this->_event_socket,  this->_body.c_str(), this->_body.size(), 0) < 0)
		std::cout << _RED _BOLD "responseError: SEND BUFFER" _END << std::endl;
	// std::cout << _PINK "RESPONSE\n" << _header << _body << _END << std::endl;
}

void	Response::executeMethod()
{
}

void	Response::sendResponse()
{
}
