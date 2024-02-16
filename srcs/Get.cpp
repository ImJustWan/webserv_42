#include "Get.hpp"

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

void	Get::sendResponse()
{
	std::ifstream	file(Response::_resource.c_str(), std::ios::binary);
	char			buffer[4096];

	std::cout << "build get response" << std::endl;
	buildHeader(file, 200);

	if (send(this->_event_socket, this->_header.c_str(), this->_header.size(), 0) < 0)
		std::cout << _RED _BOLD "Error: SEND HEADER" _END << std::endl;

	while (!file.eof())
	{
		file.read(buffer, 4096);
		if (send(this->_event_socket, buffer, file.gcount(), 0) < 0) {
			std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
		}
	}
	file.close();
}

void	Get::executeMethod()
{
	// std::cout << _LILAC _BOLD "EXECUTE Get" _END << std::endl;
	
	this->sendResponse();
}

