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

void	Post::buildResponse()
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
	if (boundaryPos == std::string::npos) {
		std::cerr << "Error: Could not find boundary." << std::endl;
		return;
	}
	size_t boundaryEnd = this->getCurrentRequest()->getRequest().find("\r\n", boundaryPos);

	boundaryPos += boundaryStart.length();
	_boundary = this->getCurrentRequest()->getRequest().substr(boundaryPos, boundaryEnd - boundaryPos);
}

bool Post::uploadChunkedFile() {

	// CREATE PATH FOR UPLOAD
	if (this->getCurrentRequest()->getLocation() != NULL)
			this->_uploadedPath = this->getCurrentRequest()->getLocation()->getUploadPath() + "/" + _filename;
		else
			this->_uploadedPath = this->getCurrentRequest()->getCurrentServer()->getRoot() + "/" + _filename;
	std::string path = this->getCurrentRequest()->getCurrentServer()->getRoot() + _uploadedPath;
	

	// CREATE FILE
	std::ofstream newFile(path.c_str());
	if (!newFile) {
		std::cerr << "Error: Could not create file" << std::endl;
		responseError(500);
		return false;
	}

	// Final string to write in file
	std::string rawFileData = "";

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
	rawFileData.append(this->getCurrentRequest()->getRequest().substr(hexStart + 5, hexSize));

	// LOOP : append each chunk until end of file / empty chunk
	while (hexSize > 0 && hexEnd < this->getCurrentRequest()->getRequest().size()) {
		hexStart = hexEnd + 2 + hexSize;
		hexEnd = this->getCurrentRequest()->getRequest().find("\n", hexStart) + 1;
		if (hexEnd == std::string::npos || hexEnd >= this->getCurrentRequest()->getRequest().size() - 1)
			break;
		hexStream.str(this->getCurrentRequest()->getRequest().substr(hexStart, hexEnd - hexStart));
		hexStream >> std::hex >> hexSize;
		rawFileData.append(this->getCurrentRequest()->getRequest().substr(hexEnd , hexSize));
	}

	// Remove last(empty) chunk / eof
	size_t dataEnd = rawFileData.rfind("0\r\n\r\n");
	if (dataEnd != std::string::npos)
		rawFileData = rawFileData.substr(0, dataEnd);

	// Check if file is too big
	if (static_cast<long long int>(rawFileData.size()) > this->getCurrentRequest()->getCurrentServer()->getClientMaxBodySize()) {
		std::cerr << "Error: File too big" << std::endl;
		return (responseError(413), false);
	}

	// Write to final file and close
	newFile.write(rawFileData.c_str(), rawFileData.size());
	newFile.close();
	return true;
}

bool Post::uploadFile() {


	// GET FILENAME
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
	// std::cout << _PINK << "Filename: " << _filename << _END << std::endl;

	// GET BOUNDARY
	extractBoundary();

	// GET FILE DATA through boundary OR chunked
	if (_boundary.empty()) {
		std::cerr << "Error: Could not extract boundary." << std::endl;
		if (this->getCurrentRequest()->getChunked() == true)
			if (uploadChunkedFile())
				return true;
		return false;
	}

	// GET FILE DATA through boundary --> uploadBoundedFile()
	size_t dataStart = this->getCurrentRequest()->getRequest().find(_boundary);
	size_t tmpStart = this->getCurrentRequest()->getRequest().find("\r\n\r\n", dataStart + _boundary.size()) + 4;
	size_t dataEnd = this->getCurrentRequest()->getRequest().rfind(_boundary);
	
	// Remove Header
	dataStart = this->getCurrentRequest()->getRequest().find("\r\n\r\n", tmpStart) + 4;
	
	if (tmpStart != std::string::npos && dataEnd != std::string::npos) {
		std::string imageData = this->getCurrentRequest()->getRequest().substr(dataStart);
		// CREATE PATH FOR UPLOAD
		if (this->getCurrentRequest()->getLocation() != NULL)
			this->_uploadedPath = this->getCurrentRequest()->getLocation()->getUploadPath() + "/" + _filename;
		else
			this->_uploadedPath = this->getCurrentRequest()->getCurrentServer()->getRoot() + "/" + _filename;
		std::string path = this->getCurrentRequest()->getCurrentServer()->getRoot() + _uploadedPath;
		
		// CREATE FILE
		std::ofstream newFile(path.c_str());
		if (!newFile) {
			std::cerr << "Error: Could not create file" << std::endl;
			responseError(500);
			return false;
		}
	
		// !!! Check if file is too big

		// Write to final file and close
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
		this->buildResponse();
}
