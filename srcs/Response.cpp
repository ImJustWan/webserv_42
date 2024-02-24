#include "Response.hpp"
#include <filesystem>

/*****************  CANONICAL FORM *****************/

Response::Response() {
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

Response::Response(Response const & src) {
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


std::string	Response::getResponse(void) const { return this->_response; }
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

}

bool	Response::extensionCheck()
{
	size_t extension = getCurrentRequest()->getResource().find_last_of('.');
	std::string ext;

	if (extension != std::string::npos) {
		ext = getCurrentRequest()->getResource().substr(extension);
		if (this->getCurrentRequest()->getCurrentServer()->getServerHandler()->getMimeMap().find(ext) == this->getCurrentRequest()->getCurrentServer()->getServerHandler()->getMimeMap().end())
			return ( false );
	}
	else if (extension == std::string::npos)
		return ( false );
	return ( true );
}

std::string	Response::trimSlash()
{
	std::string buff;
	bool hasNonSlash = false;

	for (std::string::size_type i = 0; i < getCurrentRequest()->getResource().length(); i++) {
		char currentChar = getCurrentRequest()->getResource()[i];

		if (currentChar == '/') {
			if (!hasNonSlash) {
				buff.push_back(currentChar);
			}
			hasNonSlash = true;
		} else {
			hasNonSlash = false;
			buff.push_back(currentChar);
		}
	}

	if (!buff.empty() && buff[buff.length() - 1] == '/') {
		buff.erase(buff.length() - 1);
	}

	return buff.empty() ? "/" : buff;
}

bool	Response::requestLineCheck( void )
{
	std::ifstream	file(this->getCurrentRequest()->getResource().c_str(), std::ios::binary);

	if (this->getCurrentRequest()->getResource().size() == 0)
		responseError(404);
	else if (access(this->getCurrentRequest()->getResource().c_str(), R_OK))
		responseError(403);
	else if (!file.is_open())
		responseError(404);
	// else if (this->getCurrentRequest()->getRequest().find("HTTP/1.1") == std::string::npos)
	// {
	// 	responseError(505);
	// }
	else if (this->getCurrentRequest()->getResource().find("/..") != std::string::npos
		|| getCurrentRequest()->getResource().find("../") != std::string::npos)
		responseError(403);
	else
		return ( true );
	return ( false );
}


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

	this->_response = this->_header + this->_body;
	this->getCurrentRequest()->setAsReady(true);
}

void	Response::responseError( const uint16_t & status_code )
{
	// this->getCurrentRequest()->setReadBytes(0);

	std::cout << _RED _BOLD "Response Error for : " << status_code <<  _END << std::endl;
	std::map<uint16_t, std::string>::const_iterator it = this->getCurrentRequest()->getCurrentServer()->getErrors().find(status_code);

	if (it != this->getCurrentRequest()->getCurrentServer()->getErrors().end())
	{
		char			buffer[4096];
		std::string		path = this->getCurrentRequest()->getCurrentServer()->getRoot() + it->second;
		std::ifstream	error_page(path.c_str(), std::ios::binary);
		if (!error_page){
			errorPageBuilder(status_code);
			return;
		}

		buildHeader(error_page, status_code);
		this->_response = this->_header;
		while (!error_page.eof())
		{
			error_page.read(buffer, 4096);
			this->_response += std::string(buffer, error_page.gcount());
		}
		this->getCurrentRequest()->setAsReady(true);
		error_page.close();
	}
	else
		errorPageBuilder(status_code);

}

void	Response::executeMethod()
{
}

void	Response::sendResponse()
{
}
