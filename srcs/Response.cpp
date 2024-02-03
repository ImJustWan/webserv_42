#include "Response.hpp"

Response::Response() {

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

void	Response::buildHeader( std::ifstream & file)
{
	file.seekg(0, std::ios::end);
	int file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::ostringstream file_size_str;
	file_size_str << file_size;

	// Prepare HTTP response headers
	this->_header += "HTTP/1.1";
	// this->_header += "200 OK";
	this->_header += this->_status;
	this->_header += "\r\n";
	// this->_header += "Content-Type: image/png\r\n";
	this->_header += "Content-Length: ";
	this->_header+= file_size_str.str();
	this->_header+= "\r\n";
	this->_header += "Connection: Keep-Alive\r\n";
	this->_header += "\r\n";
	// this->_header += "Keep-Alive: timeout=5, max=1000" << "\r\n";
	// std::cout << "file_size : " << file_size << std::endl;

}

/*  Chunking:
		Instead of trying to send the entire file in one go, consider breaking it into smaller chunks.
		This can help with more efficient transmission and better handling of large files.
*/
void	Response::sendResponse()
{
	std::ifstream	file(this->_ressource.c_str(), std::ios::binary);
	char			buffer[4096];

	this->_status = "200 OK";
	if (!file.is_open()) {
		// std::cerr << "Error: Could not open " << this->_ressource << std::endl;
		this->_status = "403 Forbidden";
	}

	buildHeader(file);
	if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cerr << _RED _BOLD "Error: SEND HEADER" _END << std::endl;
	while (this->_status == "200 OK" && !file.eof())
	{
		file.read(buffer, 4096);
		this->_body += buffer;

		if (send(this->_event_socket, buffer, file.gcount(), 0) < 0)
			std::cerr << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
	}
	file.close();
	std::cout << "(message sent)" << std::endl;
}


void	Response::craftResponse()
{
	Request::setRequest();
	Request::setAttributes(); // set method & resource

	// std::cout << _RIVIERA  "\nMESSAGE FROM CLIENT : \n" << std::endl;
	// std::cout << this->_request << std::endl;
	// std::cout << "END OF MESSAGE\n" _END << std::endl;
	// std::cout << "RESSOURCE : " << this->_ressource << std::endl;
	// std::cout << "( response message sent )\n" << std::endl;
}
