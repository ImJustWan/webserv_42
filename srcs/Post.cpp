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


void	Post::buildResponse()
{
	this->_response = "HTTP/1.1 201 Created\r\n\r\n";
	this->_response += HTMLBODY1;
	this->_response += HTMLBODY2;
	this->_response += HTMLBODY3;
	this->_response += _uploadedPath;
	this->_response += HTMLBODY4;
	this->getCurrentRequest()->setFinalResponse(this->_response);
	this->getCurrentRequest()->setAsReady(true);
	std::cout << _GOLD << "Uploaded file at " << this->_uploadedPath << _END << std::endl;

}

void Post::extractBoundary() {
	_boundary = "";
	std::string boundaryStart = "boundary=";

	size_t boundaryPos = this->getCurrentRequest()->getRequest().find(boundaryStart);
	if (boundaryPos == std::string::npos)
		return;
	size_t boundaryEnd = this->getCurrentRequest()->getRequest().find("\r\n", boundaryPos);

	if (boundaryEnd == std::string::npos)
		return;
	boundaryPos += boundaryStart.length();
	_boundary = this->getCurrentRequest()->getRequest().substr(boundaryPos, boundaryEnd - boundaryPos);
}

bool Post::uploadChunkedFile() {

	std::cout << _FOREST_GREEN "Uploaded POST request is chunked"  _END << std::endl;

	_rawFileData = "";

	// Remove Header
	size_t hexStart = this->getCurrentRequest()->getRequest().find("\r\n\r\n") + 4;

	// Get the first hexa size
	size_t hexEnd = this->getCurrentRequest()->getRequest().find("\n", hexStart);
	std::string hexSizeStr = this->getCurrentRequest()->getRequest().substr(hexStart, hexEnd - hexStart);
	std::stringstream hexStream(hexSizeStr);
	size_t hexSize;
	hexStream >> std::hex >> hexSize;
	hexEnd++;
	hexStart++;

	// Append first chunk
	_rawFileData.append(this->getCurrentRequest()->getRequest().substr(hexStart + 5, hexSize));

	// LOOP : append each chunk until end of file / empty chunk
	while (hexSize > 0 && hexEnd < this->getCurrentRequest()->getRequest().size()) {
		hexStart = hexEnd + 2 + hexSize;
		hexEnd = this->getCurrentRequest()->getRequest().find("\n", hexStart) + 1;
		if (hexEnd == std::string::npos || hexEnd >= this->getCurrentRequest()->getRequest().size() - 1)
			break;
		hexStream.str(this->getCurrentRequest()->getRequest().substr(hexStart, hexEnd - hexStart));
		hexStream >> std::hex >> hexSize;
		_rawFileData.append(this->getCurrentRequest()->getRequest().substr(hexEnd , hexSize));
	}

	size_t dataEnd = _rawFileData.rfind("0\r\n\r\n");
	if (dataEnd != std::string::npos)
		_rawFileData = _rawFileData.substr(0, dataEnd);
	return true;
}

bool	Post::uploadBoundedFile()
{
	std::cout << _FOREST_GREEN "Uploaded POST request is bounded"  _END << std::endl;

	size_t dataStart = this->getCurrentRequest()->getRequest().find(_boundary);
	if (dataStart == std::string::npos) {
		std::cerr << "Error: Could not find file's content" << std::endl;
		return false; // ERROR ??
	}
	size_t tmpStart = this->getCurrentRequest()->getRequest().find("\r\n\r\n", dataStart + _boundary.size()) + 4;
	size_t dataEnd = this->getCurrentRequest()->getRequest().rfind(_boundary);
	
	// Remove Header
	dataStart = this->getCurrentRequest()->getRequest().find("\r\n\r\n", tmpStart) + 4;
	
	if (tmpStart != std::string::npos && dataEnd != std::string::npos) 
		_rawFileData = this->getCurrentRequest()->getRequest().substr(dataStart);
	else {
		std::cerr << "Error: Could not find file's content" << std::endl;
		return false; // ERROR ??
	}
	return true;
}


bool	Post::createUploadFile()
{
	if (static_cast<long long int>(_rawFileData.size()) > this->getCurrentRequest()->getCurrentServer()->getClientMaxBodySize())
		return (responseError(413), false);
	std::ofstream newFile(_fullPath.c_str());
	if (!newFile) {
		std::cerr << "Error: Could not create Upload File" << std::endl;
		return (responseError(500), false);
	}

	// Write to final file and close
	newFile.write(_rawFileData.c_str(), _rawFileData.size());
	newFile.close();
	return true;
}


void	Post::createUploadPath()
{
	if (this->getCurrentRequest()->getLocation() != NULL)
		this->_uploadedPath = this->getCurrentRequest()->getLocation()->getUploadPath() + "/" + _filename;
	else
		this->_uploadedPath = this->getCurrentRequest()->getCurrentServer()->getRoot() + "/" + _filename;
	_fullPath = this->getCurrentRequest()->getCurrentServer()->getRoot() + _uploadedPath;
}

void	Post::createFilename()
{
	size_t filenameStart = this->getCurrentRequest()->getRequest().find("filename=\"");// + 10;
	if (filenameStart == std::string::npos){
		srand(time(NULL));
		int rnd = rand();
		std::stringstream	filename;
		filename << rnd;
		_filename = filename.str();
	}
	else {
		filenameStart += 10;
		size_t filenameEnd = this->getCurrentRequest()->getRequest().find("\"", filenameStart);
		_filename = this->getCurrentRequest()->getRequest().substr(filenameStart, filenameEnd - filenameStart);
	}
}


bool Post::uploadFile() {

	createFilename();

	extractBoundary();

	// GET CHUNKED FILE DATA
	if (_boundary.empty()) {
		if (this->getCurrentRequest()->getChunked() == true)
			if (uploadChunkedFile())
				return true;
		return false;
	}

	// GET BOUNDED FILE DATA
	if (uploadBoundedFile() == false)
		return false;

	createUploadPath();
	if (createUploadFile() == false)
		return false;
	return true;
}

void	Post::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;

	if (uploadFile())
		this->buildResponse();
	// else ? error 
}
