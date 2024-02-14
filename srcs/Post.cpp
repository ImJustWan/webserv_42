# include "Post.hpp"
# include "CGI.hpp"

Post::Post() {

}

Post::~Post() {

}

Post& Post::operator= ( const Post& cpy ) {
	(void)cpy;
	return *this;
}

// std::string		Post::extractToken( char *searched, size_t & found )
// {
// 	std::string	token;
// 	const size_t	pos 	= _request.find(searched, found);
// 	const size_t	lenght 	= _request.find("\n", pos) - pos - std::strlen(searched);

// 	token = _request.substr(pos + std::strlen(searched), lenght);
// 	found = _request.find(token, found) + lenght;
// 	// std::cout << "token : " << token << std::endl;
// 	// std::cout << "found : " << found << std::endl;
// 	// _request.erase(pos, lenght);

// 	return ( token );
// }

// std::string	Post::getKey(size_t & found )
// {
// 	return ( extractToken((char *)"name=", found) );
// }

// std::string	Post::getValue(size_t & found, std::string const & queryKey)
// {
// 	std::string	value;
// 	size_t		lenght;

// 	found += 3;
// 	lenght = _request.find(queryKey, found) - found - 3;
// 	value = _request.substr(found, lenght);
// 	found = _request.find(value, found) + lenght;

// 	return ( value );
// }

// void	Post::fillMap( void )
// {
// 	std::string		queryKey;
// 	size_t			found = 0;
// 	std::string		key;
// 	std::string		value;

// 	queryKey = extractToken((char *)"boundary=", found);
// 	found = _request.find(queryKey);
// 	while ( found != std::string::npos )
// 	{
// 		key		= getKey(found);
// 		value	= getValue(found, queryKey);
// 		_post_map[key] = value;
// 		found = _request.find(queryKey, found);
// 	}

// 	// for ( std::map<std::string, std::string>::iterator it = _post_map.begin(); it != _post_map.end(); it++ ) {
// 	// 	std::cout	<< "key   : " << it->first << std::endl
// 	// 				<< "value : " << it->second	 << std::endl;
// 	// }
// }

void	Post::buildHeader()
{
	this->_header = "HTTP/1.1 ";
	this->_header += "202 Accepted";
	this->_header += "\r\n";
	this->_header += "Content-Type: text/plain\r\n";
	// this->_header += "Connection: Keep-Alive\r\n";

	this->_header += "\r\n";
}

void	Post::sendResponse()
{
	this->buildHeader();
	if (_finished == true)
		this->_body = "The request has been completed, file is uploaded. Thanks :)\n";
	else
		this->_body = "The request has been accepted for processing. Check back later for results.\n";

	if (send(this->_event_socket,  this->_body.c_str(), this->_body.size(), 0) < 0)
		std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;

}

// UPLOAD TO DO ENTIRELY
// if upload location, then there
// else upload to root/upload

void	Post::writeFile()
{
		std::string path = this->_current_server->getRoot() + "/OUTPUUUUT";
		std::ofstream newFile(path.c_str());
		newFile.write(_request.c_str(), _request.size());
		newFile.close();
}

void	Post::executeMethod()
{
	// std::cout << "EXECUTE Post" << std::endl;
	// std::cout << "RESSOURCE : " << Response::_resource << std::endl;
	// std::cout << "Request :: " << _request << std::endl;
	// fillMap();
	writeFile();
	this->sendResponse();
}
