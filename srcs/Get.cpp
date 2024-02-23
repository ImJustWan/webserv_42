#include "Get.hpp"
#include <dirent.h>


/*****************  CANONICAL FORM *****************/

Get::Get() {
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

	this->getCurrentRequest()->setAsReady(true);

}

void	Get::sendResponse()
{
	std::ifstream	file(this->getCurrentRequest()->getResource().c_str(), std::ios::binary);
	char			buffer[4096];

	buildHeader(file, 200);
	if (send(this->getCurrentRequest()->getEventSocket(), this->_header.c_str(), this->_header.size(), 0) < 0)
		this->getCurrentRequest()->setLastEvent(0);
	while (!file.eof())
	{
		file.read(buffer, 4096);
		if (send(this->getCurrentRequest()->getEventSocket(), buffer, file.gcount(), 0) < 0)
			this->getCurrentRequest()->setLastEvent(0);
	}
	file.close();
}


bool	Get::checkResource()
{
	struct stat	buffer;

	this->getCurrentRequest()->setResource(this->trimSlash());
	if (this->getCurrentRequest()->getResource().find(this->getCurrentRequest()->getRoot()) == std::string::npos)
		this->getCurrentRequest()->setResource(this->getCurrentRequest()->getRoot() + this->getCurrentRequest()->getResource());

	if (this->getCurrentRequest()->getResource().at(0) == '/')
		this->getCurrentRequest()->setResource(this->getCurrentRequest()->getResource().substr(1));

	if (stat(this->getCurrentRequest()->getResource().c_str(), &buffer))
	{
		std::cout << "stat failed on " << this->getCurrentRequest()->getResource() << std::endl;
		return (responseError(404), false);
	}
	
	if (S_ISREG(buffer.st_mode))
		return true;
	if (S_ISDIR(buffer.st_mode))
	{
		if (this->getCurrentRequest()->getIndex() == "")
		{
			if ((this->getCurrentRequest()->getLocation() != NULL && this->getCurrentRequest()->getLocation()->getAutoindex())
				|| (this->getCurrentRequest()->getLocation() == NULL && this->getCurrentRequest()->getCurrentServer()->getAutoindex()))
				return (getAutoIndex(), false);
			else
				return (responseError(403), false);
		}
		else if (stat((this->getCurrentRequest()->getResource() + "/" + this->getCurrentRequest()->getIndex()).c_str(), &buffer))
		{
			std::cout << "second stat failed for : " << this->getCurrentRequest()->getResource() + this->getCurrentRequest()->getIndex() << std::endl;
			return (responseError(404), false);
		}
		else if (S_ISREG(buffer.st_mode))
		{
			this->getCurrentRequest()->setResource(this->getCurrentRequest()->getResource() + "/" + this->getCurrentRequest()->getIndex());
			return (true);
		}
		else
			return false;
	}
	return false;
}


void	Get::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;
	
	if (checkResource() && requestLineCheck())
		this->sendResponse();
}
