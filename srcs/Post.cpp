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

void Post::extractBoundary() {
	_boundary = "";
	std::string boundaryStart = "boundary=";

	size_t boundaryPos = _request.find(boundaryStart);
	size_t boundaryEnd = _request.find("\r\n", boundaryPos);

	boundaryPos += boundaryStart.length();
	_boundary = _request.substr(boundaryPos, boundaryEnd - boundaryPos);
}

void Post::writeFile() {

	extractBoundary();

	if (_boundary.empty()) {
		std::cerr << "Error: Could not extract boundary." << std::endl;
		return;
	}

	size_t dataStart = _request.find(_boundary);
	size_t tmpStart = _request.find("\r\n\r\n", dataStart + _boundary.size()) + 4;
	size_t dataEnd = _request.rfind(_boundary);
	
	dataStart = _request.find("\r\n\r\n", tmpStart) + 4;
	
	size_t filenameStart = _request.find("filename=\"") + 10;
	size_t filenameEnd = _request.find("\"", filenameStart);
	_filename = _request.substr(filenameStart, filenameEnd - filenameStart);

	if (tmpStart != std::string::npos && dataEnd != std::string::npos) {
		std::string imageData = _request.substr(dataStart);
		std::string path = this->_current_server->getRoot() + "/" + _filename;
		std::ofstream newFile(path.c_str());
		newFile.write(imageData.c_str(), imageData.size());
		newFile.close();
	}
	else {
		std::cerr << "Error: Could not find file's content" << std::endl;
	}
}

void	Post::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;

	writeFile();
	this->sendResponse();
}
