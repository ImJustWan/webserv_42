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

std::string Post::extractBoundary() {
	std::string boundary = "";
	std::string boundaryPrefix = "boundary=";

	size_t boundaryPos = _request.find(boundaryPrefix);
	// boundaryPos += boundaryPrefix.length();
	size_t boundaryEnd = _request.find("\r\n", boundaryPos);

	boundaryPos += boundaryPrefix.length();
	boundary = _request.substr(boundaryPos, boundaryEnd - boundaryPos);

	// Remove any leading or trailing whitespace or separators
	boundary.erase(std::remove_if(boundary.begin(), boundary.end(), ::isspace), boundary.end());
	boundary.erase(std::remove(boundary.begin(), boundary.end(), ';'), boundary.end());
	// std::cout << "_request: \n" << _request << std::endl;
	// std::cout << "\nBOUNDARY: \n" << boundary << std::endl;
	return (boundary);
}

void Post::writeFile() {
	// Extract the boundary from the request
	std::string boundary = extractBoundary();

	if (boundary.empty()) {
		std::cerr << "Error: Could not extract boundary." << std::endl;
		return;
	}

	size_t boundaryStart = _request.find(boundary);
	// Find the start and end positions after the boundary
	size_t dataStart = _request.find("\r\n\r\n", boundaryStart) + 4;
	size_t dataEnd = _request.rfind(boundary);

	if (dataStart != std::string::npos && dataEnd != std::string::npos) {
		// std::cout << "DATA START: " << dataStart << std::endl;
	
		// Extract the binary data without the boundary at the end
		size_t filenameStart = _request.find("filename=\"") + 10;
		size_t filenameEnd = _request.find("\"", filenameStart);
		std::string filename = _request.substr(filenameStart, filenameEnd - filenameStart);

		std::string imageData = _request.substr(dataStart);
		// std::cout << "IMAGE DATA SIZE : " << imageData.size() << std::endl;
		// std::cout << "_request\n\n" << _request << std::endl;

		// Write the binary data to a file
		std::string path = this->_current_server->getRoot() + "/" + filename;
		std::ofstream newFile(path.c_str(), std::ios::binary);
		// std::cout << "IMAGE DATA: \n\n" << imageData << std::endl;
		newFile.write(imageData.c_str(), imageData.size());
		newFile.close();

		// Debug messages
		std::cout << "File created: " << path << std::endl;
	} else {
		std::cerr << "Error: Could not find boundary positions." << std::endl;
	}
}

void	Post::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;

	writeFile();
	this->sendResponse();
}
