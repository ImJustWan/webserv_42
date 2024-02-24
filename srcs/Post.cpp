# include "Post.hpp"
# include "uploaded.hpp"

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
	this->_header += "202 Accepted\r\n\r\n";
}

void	Post::sendResponse()
{
	this->buildHeader();
	this->_response = this->_header;
	this->_response += HTMLBODY1;
	this->_response += HTMLBODY2;
	this->_response += HTMLBODY3;
	this->_response += this->getCurrentRequest()->getCurrentServer()->getUploadPath() + "/" + _filename;
	this->_response += HTMLBODY4;
	this->getCurrentRequest()->setAsReady(true);
	std::cout << _GOLD << "Uploaded file at " << this->_uploadedPath << _END << std::endl;

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

	if (tmpStart != std::string::npos && dataEnd != std::string::npos) {
		std::string imageData = this->getCurrentRequest()->getRequest().substr(dataStart);
		this->_uploadedPath = this->getCurrentRequest()->getCurrentServer()->getUploadPath() + "/" + _filename;
		std::string path = this->getCurrentRequest()->getCurrentServer()->getRoot() + _uploadedPath;
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

	uploadFile();
	this->sendResponse();
}
