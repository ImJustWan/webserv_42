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
	this->_body = "The request has been completed, file is uploaded. Thanks :)\n";
	this->_response = this->_header + this->_body;
	this->_header += this->_body;
	
	this->getCurrentRequest()->setAsReady(true);

}

void Post::extractBoundary() {
	_boundary = "";
	std::string boundaryStart = "boundary=";

	size_t boundaryPos = this->getCurrentRequest()->getRequest().find(boundaryStart);
	size_t boundaryEnd = this->getCurrentRequest()->getRequest().find("\r\n", boundaryPos);

	boundaryPos += boundaryStart.length();
	_boundary = this->getCurrentRequest()->getRequest().substr(boundaryPos, boundaryEnd - boundaryPos);
}

void Post::uploadFile() {

	extractBoundary();

	if (_boundary.empty()) {
		std::cerr << "Error: Could not extract boundary." << std::endl;
		return;
	}

	size_t dataStart = this->getCurrentRequest()->getRequest().find(_boundary);
	size_t tmpStart = this->getCurrentRequest()->getRequest().find("\r\n\r\n", dataStart + _boundary.size()) + 4;
	size_t dataEnd = this->getCurrentRequest()->getRequest().rfind(_boundary);
	
	dataStart = this->getCurrentRequest()->getRequest().find("\r\n\r\n", tmpStart) + 4;
	
	size_t filenameStart = this->getCurrentRequest()->getRequest().find("filename=\"") + 10;
	size_t filenameEnd = this->getCurrentRequest()->getRequest().find("\"", filenameStart);
	_filename = this->getCurrentRequest()->getRequest().substr(filenameStart, filenameEnd - filenameStart);

	/* UPLOAD PATH MUST BE upload_path if it exists */

	if (tmpStart != std::string::npos && dataEnd != std::string::npos) {
		std::string imageData = this->getCurrentRequest()->getRequest().substr(dataStart);
		std::string path = this->getCurrentRequest()->getCurrentServer()->getRoot() + "/" + _filename;
		std::ofstream newFile(path.c_str());
		// std::cout << _PINK "Request " << this->getCurrentRequest()->getRequest() << _END <<  std::endl;
		newFile.write(imageData.c_str(), imageData.size());
		newFile.close();
		std::cerr << "File created at " << path << std::endl;
	}
	else {
		std::cerr << "Error: Could not find file's content" << std::endl;
	}
}

void	Post::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;

	uploadFile();
	this->sendResponse();
}
