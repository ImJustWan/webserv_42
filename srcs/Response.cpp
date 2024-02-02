#include "Response.hpp"

Response::Response() {

}

Response::~Response() {

}

Response::Response ( const Response& cpy ) {
	*this = cpy;
}

Response& Response::operator= ( const Response& cpy ) {
	(void)cpy;
	return *this;
}

void	Response::buildHeader( std::ifstream & file, std::ostringstream & headers )
{
	file.seekg(0, std::ios::end);
	int file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Prepare HTTP response headers
	headers << "HTTP/1.1 200 OK\r\n";
	// headers << "Content-Type: image/png\r\n";
	headers << "Content-Length: " << file_size << "\r\n";
	headers << "Connection: Keep-Alive" << "\r\n";
	headers << "\r\n";
	// headers << "Keep-Alive: timeout=5, max=1000" << "\r\n";
	// std::cout << "file_size : " << file_size << std::endl;

}

/*  Chunking:
		Instead of trying to send the entire file in one go, consider breaking it into smaller chunks.
		This can help with more efficient transmission and better handling of large files.
*/
void	Response::sendFile(const std::string & file_path, int const & _baby_socket )
{
	std::ostringstream headers;
	std::ifstream	file(file_path.c_str(), std::ios::binary);
	char			buffer[4096];

	if (!file.is_open()) {
		std::cerr << "Error: Could not open " << file_path << std::endl;
		return ;
	}

	buildHeader(file, headers);
	if (send(_baby_socket, headers.str().c_str(), headers.str().size(), 0) < 0)
		std::cerr << _RED _BOLD "Error: SEND HEADER" _END << std::endl;
	// std::cerr << "Header :" << headers.str() << std::endl;

	while ( !file.eof() )
	{
		file.read(buffer, 4096);
		if (send(_baby_socket, buffer, file.gcount(), 0) < 0)
			std::cerr << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
	}
	file.close();
}


void	Response::sendResponse(int const & _baby_socket )
{
	std::string	request;



	Request::setRequest( _baby_socket );
	Request::setAttributes();
	request = Request::getRequest();

	// struct epoll_event modifiedList;

	// modifiedList.events = EPOLLOUT;
	// if (epoll_ctl(this->_epfd, EPOLL_CTL_MOD, _baby_socket, &modifiedList) == -1)
	// 	std::cout << "EPOLLOUT epoll error" << std::endl;

	// std::cout << _RIVIERA  "\nMESSAGE FROM CLIENT : \n" << std::endl;
	// std::cout << request << std::endl;
	// std::cout << "END OF MESSAGE\n" _END << std::endl;
	// std::cout << "RESSOURCE : " << Request::_ressource << std::endl;

	sendFile(Request::_ressource, _baby_socket);
	std::cout << "( response message sent )\n" << std::endl;
}

