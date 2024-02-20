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

	std::cout << _GOLD "Link is : " << ss.str() << _END << std::endl;
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
	// std::cout << "Autoindex is ON BABYYYYY" << std::endl;


	this->_body = getPage(_resource.c_str(), _host, _listen);
	
	std::ostringstream file_size_str;
	file_size_str << _body.size();
	
	this->_header = "HTTP/1.1 200  OK\r\n";
	this->_header += "Content-Type: text/html\r\n";
	this->_header += "Content-Length: ";
	this->_header += file_size_str.str();
	this->_header += "\r\n\r\n";
	this->_header += this->_body;
	if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cout << _RED _BOLD "Error: SEND HEADER" _END << std::endl;
}


void	Get::sendResponse()
{
	std::ifstream	file(Response::_resource.c_str(), std::ios::binary);
	char			buffer[4096];

	buildHeader(file, 200);
	if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cout << _RED _BOLD "Error: SEND HEADER" _END << std::endl;
	while (!file.eof())
	{
		file.read(buffer, 4096);
		// std::cout << "Event socket is : " << this->_event_socket << std::endl;
		if (send(this->_event_socket, buffer, file.gcount(), 0) < 0) {
			std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
		}
	}
	file.close();
}


bool	Get::checkResource()
{
	struct stat	buffer;

	_resource = this->trimSlash();
	if (_resource.find(_root) == std::string::npos)
		_resource = _root + _resource;

	if (_resource.at(0) == '/')
		_resource = _resource.substr(1);
	// std::cout << _EMERALD "Resource is now : " << _resource << _END << std::endl;

	if (stat(_resource.c_str(), &buffer))
	{
		std::cout << "stat failed on " << _resource << std::endl;
		return (responseError(404), false);
	}
	
	if (S_ISREG(buffer.st_mode))
		return true;
	if (S_ISDIR(buffer.st_mode))
	{
		std::cout << "Resource is a directory" << std::endl;
		if (this->getIndex() == "")
		{
			std::cout << "No index found" << std::endl;
			// std::cout << _PINK << _request << _END << std::endl;
			if ((this->getLocation() != NULL && this->getLocation()->getAutoindex())
				|| (this->getLocation() == NULL && this->getCurrentServer()->getAutoindex()))
			{

				return (getAutoIndex(), false);
			}	
			else
			{

				return (responseError(403), false);
			}
		}
		else if (stat((_resource + "/" + this->getIndex()).c_str(), &buffer))
		{
			std::cout << "second stat failed for : " << _resource + this->getIndex() << std::endl;
			return (responseError(404), false);
		}
		else if (S_ISREG(buffer.st_mode))
		{
			_resource = _resource + "/" + this->getIndex();
			std::cout << _FOREST_GREEN "Resource is now : " << _resource << _END << std::endl;
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
	
	// std::cout << "Resource is : " << _resource << std::endl;
	if (checkResource() && requestLineCheck())
		this->sendResponse();
}
