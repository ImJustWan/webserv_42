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


void Get::getAutoIndex()
{
	std::cout << "Autoindex is ON BABYYYYY" << std::endl;

	DIR *dir;

	struct dirent *ent;
	struct stat buffer;
	if ((dir = opendir(_resource.c_str())) != NULL)
	{
		this->_header = "HTTP/1.1 200 OK\r\n";
		this->_header += "Content-Type: text/html\r\n\r\n";

		this->_header += "<html>\r\n<head>\r\n<title>Index of /forms/</title>\r\n</head>\r\n";
		this->_header += "<body>\r\n<h1>Index of /forms/</h1>\r\n<hr>\r\n<pre>\r\n";
		this->_header += "<a href=\"../\">../</a>\r\n<a href=\"kill_form/\">kill_form/</a>\r\n";
		this->_header += "<a href=\"love_form/\">love_form/</a>\r\n";
		this->_header += "<a href=\"upload_form/\">upload_form/</a>\r\n</pre>\r\n<hr>\r\n</body>\r\n</html>\r\n";
		while ((ent = readdir(dir)) != NULL)
		{
			/* TO DO :
				fix looping ! 
				fix redir to avoid concatenating path
			*/
			std::cout << "looping" << std::endl;
			std::string		fileName(ent->d_name);
			if (stat((_resource + "/" + fileName).c_str(), &buffer))
			{
				std::cerr << "stat failed on " << _resource + "/" + fileName << std::endl;
				responseError(500);
			}
		}

		closedir(dir);

		this->_header += "</ul></body></html>\n";
		if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
			std::cout << _RED _BOLD "Error: SEND HEADER" _END << std::endl;
	}
	else
	{
		std::cerr << "Error opening directory: " << _resource << std::endl;
	}
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

	// std::cout << _EMERALD "Resource is : " << _resource << _END << std::endl;
	this->trimSlash();
	_resource = _root + _resource;
	// std::cout << _EMERALD "Resource is now : " << _resource << _END << std::endl;

	/* TO DO :
		check content type
		mim type ? */

	if (stat(_resource.c_str(), &buffer))
	{
		std::cout << "stat failed on " << _resource << std::endl;
		return (responseError(404), false);
	}
	if (S_ISREG(buffer.st_mode))
		return true;
	if (S_ISDIR(buffer.st_mode))
	{
		// std::cout << "Resource is a directory" << std::endl;
		if (this->getIndex() == "")
		{
			std::cout << "No index found" << std::endl;
			// std::cout << _PINK << _request << _END << std::endl;
			if (this->getLocation() != NULL && this->getLocation()->getAutoindex() == true)
				return (getAutoIndex(), false);
			else
				return (responseError(403), false);
		}
		else if (stat((_resource + "/" + this->getIndex()).c_str(), &buffer))
		{
			std::cout << "second stat failed" << std::endl;
			return (responseError(500), false);
		}
		else if (S_ISREG(buffer.st_mode))
		{
			// std::cout << "okok ta mere "  << this->getIndex() << std::endl;
			_resource = _resource + "/" + this->getIndex();
			// std::cout << _EMERALD "Resource is now : " << _resource << _END << std::endl;
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
