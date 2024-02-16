# include "Post.hpp"

/*****************  CANONICAL FORM *****************/

Post::Post() {
}

Post::~Post() {
}

Post::Post(Post const & src) : Response(src) {
	*this = src;
}

Post& Post::operator=(const Post& cpy) {
	if (this == &cpy)
		return (*this);
	Response::operator=(cpy);
	return *this;
}


/*****************  CLASS METHODS *****************/

void	Post::buildHeader()
{
	this->_header = "HTTP/1.1 ";
	this->_header += "202 Accepted";
	this->_header += "\r\n";
	this->_header += "Content-Type: text/plain\r\n";
	this->_header += "\r\n";
}

void	Post::sendResponse()
{
	this->buildHeader();
	if (_finished == true)
		this->_body = "The request has been completed, file is uploaded. Thanks :)\n";
	else
		this->_body = "The request has been accepted for processing. Check back later for results.\n";

	this->_header += this->_body;
	if (send(this->_event_socket,  this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;

}

void	Post::writeFile()
{
		std::string path = this->_current_server->getRoot() + "/OUTPUUUUT";
		std::ofstream newFile(path.c_str());
		newFile.write(_request.c_str(), _request.size());
		newFile.close();
}

void	Post::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;

	writeFile();
	this->sendResponse();
}
