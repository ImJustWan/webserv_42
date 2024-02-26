#include "Get.hpp"
#include <dirent.h>


/*****************  CANONICAL FORM *****************/

Get::Get() : _buffer(READ_BUFFER_SIZE) {
}

Get::~Get() {
}

Get::Get(Get const & src) : Response(src) {
	*this = src;
}

Get& Get::operator=(const Get& cpy) {
	if (this == &cpy)
		return (*this);
	Response::operator=(cpy);
	return *this;
}


/*****************  CLASS METHODS *****************/

std::string	getLink(std::string const &dirEntry, std::string const &dirName, std::string const &host, int port) {

	std::ostringstream linkStream;
	linkStream << "http://" << host << ":" << port << dirName;

	// Add a "/" if dirEntry is a directory
	if (dirEntry[0] != '.')
		linkStream << (dirEntry[dirEntry.length() - 1] == '/' ? "" : "/") << dirEntry;

	std::string link = linkStream.str();

	std::stringstream ss;
	ss << "\t\t<p><a href=\"" << link << "\">" << dirEntry << "</a></p>\n";

	// std::cout << _GOLD "Link is : " << ss.str() << _END << std::endl;
	return ss.str();
}

std::string	getPage(const char *path, std::string const &host, int port) {

	std::string dirName(path);
	DIR *dir = opendir(path);
	std::string page =	"<!DOCTYPE html>\n<html>\n<head>\n<title>" + dirName + "</title>\n</head>\n<body>\n<h1>INDEX</h1>\n<p>\n";

	if (dir == NULL) {
		std::cerr << _RED << "Error: could not open [" << path << "]" << _END << std::endl;
		return "";
	}
	if (dirName[0] != '/')
		dirName = "/" + dirName;
	for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) {
		if (dirEntry->d_name[0] == '.' )
			continue;
		page += getLink(std::string(dirEntry->d_name), dirName, host, port);
	}
	page +="</p>\n</body>\n</html>\n";
	closedir(dir);
	return page;
}
	
void Get::getAutoIndex()
{
	this->_body = getPage(this->getCurrentRequest()->getResource().c_str(), getCurrentRequest()->getHost(), getCurrentRequest()->getListen());
	
	std::ostringstream file_size_str;
	file_size_str << _body.size();
	
	this->_header = "HTTP/1.1 200  OK\r\n";
	this->_header += "Content-Type: text/html\r\n";
	this->_header += "Content-Length: ";
	this->_header += file_size_str.str();
	this->_header += "\r\n\r\n";
	this->_response = this->_header + this->_body;
	this->getCurrentRequest()->setFinalResponse(this->_response);

	this->getCurrentRequest()->setAsReady(true);

}


void Get::readChunk() {

	if (!_file.is_open()) {
		std::cout <<  _RED _BOLD "Error: File not open." _END << std::endl;
		return;
	}

	char buffer	[READ_BUFFER_SIZE];
	this->_file.read(buffer, READ_BUFFER_SIZE);
	this->_response.append(buffer, _file.gcount());

	if (_file)
	{
		this->getCurrentRequest()->setSentFinished(false);
		this->getCurrentRequest()->setAsReady(false);
	} else {
		_file.clear(); // Clear the error state
		_file.close();
		this->getCurrentRequest()->setSentFinished(true);
		this->getCurrentRequest()->setAsReady(true);
		this->getCurrentRequest()->setFinalResponse(this->_response);

	}

}

void	Get::handleFile()
{
	if (!_file.is_open()) {
		// std::cout << _FOREST_GREEN "Opening file at " << this->getCurrentRequest()->getResource() << _END << std::endl;
		_file.open(this->getCurrentRequest()->getResource().c_str(), std::ios::binary);
		if (!_file.is_open()) {
			// std::cerr << "Error opening file." << std::endl;
			return;
		}
		if (this->getCurrentRequest()->getLocation() && !this->getCurrentRequest()->getLocation()->getRewrite().empty())
		{
			buildHeader(_file, this->getCurrentRequest()->getLocation()->getRewrite().begin()->first);
		}

		else
			buildHeader(_file, 200);
		if (this->_response.size() == 0){
			this->_response = this->_header;
		}
	}

	readChunk();
}



void	Get::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;
	
	if (checkResource() && requestLineCheck())
		this->handleFile();
}
