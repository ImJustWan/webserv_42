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
	if (this->getCurrentRequest()->getLocation() != NULL)
		this->_response += this->getCurrentRequest()->getLocation()->getUploadPath() + "/" + _filename;
	else
		this->_response += this->getCurrentRequest()->getCurrentServer()->getRoot() + "/" + _filename;
	this->_response += HTMLBODY4;
	this->getCurrentRequest()->setFinalResponse(this->_response);
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

bool Post::uploadFile() {

	extractBoundary();

	if (_boundary.empty()) {
		std::cerr << "Error: Could not extract boundary." << std::endl;
		return false;
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
		// this->_uploadedPath = this->getCurrentRequest()->getCurrentServer()->getUploadPath() + "/" + _filename;
		if (this->getCurrentRequest()->getLocation() != NULL)
			this->_uploadedPath = this->getCurrentRequest()->getLocation()->getUploadPath() + "/" + _filename;
		else
			this->_uploadedPath = this->getCurrentRequest()->getCurrentServer()->getRoot() + "/" + _filename;
		std::string path = this->getCurrentRequest()->getCurrentServer()->getRoot() + _uploadedPath;
		std::ofstream newFile(path.c_str());
		if (!newFile) {
			std::cerr << "Error: Could not create file" << std::endl;
			responseError(500);
			return false;
		}
		newFile.write(imageData.c_str(), imageData.size());
		newFile.close();
	}
	else {
		std::cerr << "Error: Could not find file's content" << std::endl;
	}
	return true;
}

void	Post::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;

	if (uploadFile())
		this->sendResponse();
}
