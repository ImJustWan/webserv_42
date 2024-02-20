# include "Request.hpp"
# include "Response.hpp"
# include "Get.hpp"
# include "Post.hpp"
# include "Delete.hpp"
# include "CGI.hpp"

/*****************  CANONICAL FORM *****************/

Request::Request() {
	_epfd = 0;
	_event_socket = 0;
	_request = "";
	_method = "";
	_resource = "";
	_valread = 0;
	_socketState = READ_STATE;
	_response = NULL;
	_location = NULL;
	_finished = false;
	_readBytes = 0;
	_contentLength = 0;
}

Request::~Request() {
	if (_response)
		delete _response;
}

Request::Request ( const Request& src ) : IEvent(src) {
	*this = src;
}

Request& Request::operator= ( const Request& cpy ) {
	if (this == &cpy)
		return (*this);
	this->_socketState = cpy._socketState;
	this->_current_server = cpy._current_server;
	this->_response = cpy._response;
	this->_epfd = cpy._epfd;
	this->_event_socket = cpy._event_socket;
	this->_request = cpy._request;
	this->_method = cpy._method;
	this->_resource = cpy._resource;
	this->_valread = cpy._valread;
	this->_finished = cpy._finished;
	this->_readBytes = cpy._readBytes;
	this->_methods = cpy._methods;
	return *this;
}


/* ****************  GETTERS **************** */

int const &	Request::getEpfd(void) const { return (this->_epfd); }
int const &	Request::getValread(void) const { return (this->_valread); }
int const &	Request::getEventSocket(void) const { return (this->_event_socket); }
int const &	Request::getMethods(void) const { return (this->_methods); }
int const &	Request::getListen(void) const { return (this->_listen); }
std::string	const & Request::getRequest(void) const { return (this->_request); }
std::string	const & Request::getMethod(void) const { return (this->_method); }
std::string const & Request::getResource(void) const { return ( this->_resource ); }
Server* Request::getCurrentServer(void) const { return ( this->_current_server ); }
Location* Request::getLocation() const { return ( this->_location ); }
ServerHandler* Request::getServerHandler() const { return ( this->_serverHandler ); }
std::string const & Request::getIndex(void) const { return ( this->_index ); }
std::string const & Request::getHost(void) const { return ( this->_host ); }
std::string const & Request::getRoot(void) const { return ( this->_root ); }


/* ****************  SETTERS **************** */

void	Request::setEpfd( const int epfd ) { this->_epfd = epfd; }
void	Request::setMethods( const int methods ) { this->_methods = methods; }
void	Request::setListen( const int listen ) { this->_listen = listen; }
void	Request::setEventSocket( const int socket ) { this->_event_socket = socket; }
void	Request::setValread( const int valread ) { this->_valread = valread; }
void	Request::setSocketState( bool state ) { this->_socketState = state; }
void	Request::setCurrentServer( Server *current ) { this->_current_server = current; }
void	Request::setLocation( Location *location ) { this->_location = location; }
void	Request::setServerHandler( ServerHandler *serverHandler ) { this->_serverHandler = serverHandler; }
void	Request::setRoot( std::string root ) { this->_root = root; }
void	Request::setIndex( std::string index ) { this->_index = index; }
void	Request::setHost( std::string host ) { this->_host = host; }
void	Request::setResource( std::string resource ) { this->_resource = resource; }
void	Request::setRequest( std::string request ) { this->_request = request; }

size_t Request::findContentLength( size_t const & found ) const
{
	const size_t	length = _request.find("\n", found) - found;
	const size_t	content_length = std::atoi(_request.substr(found, length).c_str());

	return ( content_length );
}

void	Request::findLocation()
{
	if (this->_current_server == NULL)
		return ;
	if (this->_current_server->getLocations().empty())
	{
		this->setLocation(NULL);
		return ;
	}
	std::map<std::string, Location *>	locations = this->_current_server->getLocations();
	
	std::string				resource = this->getResource();
	std::string 			root = "/";
	
	root += this->_current_server->getRoot();
	
	if (resource.find(root) == 0)
		resource.erase(0, root.length());

	size_t					size = resource.size();

	for (size_t i = 0; i < size; i++)
	{
		if (locations.find(resource) != locations.end())
		{
			// std::cout << _LILAC "Location found : " << resource << _END << std::endl;
			this->setLocation(locations[resource]);
			return ;
		}
		resource.resize(resource.size() - 1);
	}
	this->setLocation(NULL);
}

static void	bytesConcat(std::string & s1, char *s2, int size)
{
	int	j = 0;
	int	i = s1.size();

	s1.resize(s1.size() + size);
	for (; j < size; i++) {
		s1[i] = s2[j];
		j++;
	}
}

void Request::setRequest() {
	char buffer[4096] = "";

	_valread = recv(this->_event_socket, buffer, sizeof(buffer) - 1, 0);
	_readBytes += _valread;
	bytesConcat(_request, buffer, _valread);

	size_t headerEnd = _request.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
		_finished = true;

	size_t found = _request.find("Content-Length:");
	_contentLength = (found != std::string::npos) ? findContentLength(found + 1 + std::strlen("Content-Length:")) : _valread;

	// std::cout << _LILAC "Content-Length: " << _contentLength << _END << std::endl;
	
	if (_readBytes < _contentLength)
		_finished = false;
	else
	{
		if (_method == "POST" || headerEnd != std::string::npos)
			_finished = true;
		_readBytes = 0;
	}
}

void	Request::setMethodsRootIndex()
{

	if (this->getLocation() != NULL)
		this->setMethods(this->getLocation()->getMethods());
	else
		this->setMethods(this->getCurrentServer()->getMethods());

	if (this->getLocation() != NULL && this->getLocation()->getRoot() != "")
		this->setRoot(this->getLocation()->getRoot());
	else if (this->getLocation() != NULL)
		this->setRoot("");
	else 
		this->setRoot(this->getCurrentServer()->getRoot());
	
	if (this->getLocation() != NULL && this->getLocation()->getIndex() != "")
		this->setIndex(this->getLocation()->getIndex());
	else if (this->getLocation() != NULL)
		this->setIndex("");
	else
		this->setIndex(this->getCurrentServer()->getIndex());
	
	// std::cout << _LILAC "Root is : " << this->getRoot() << _END << std::endl;
}

void	Request::setAttributes()
{
	std::istringstream	iss(Request::_request);

	iss >> _method >> _resource;

	std::string line;

	_listen = 0;

	// retrieve host and port
	while (std::getline(iss, line)) {
		if (line.find("Host:") != std::string::npos) {
			std::istringstream hostLine(line);
			hostLine.ignore(256, ':');
			hostLine >> _host;

			size_t pos = _host.find(':');
			if (pos != std::string::npos) {
				std::istringstream portStream(_host.substr(pos + 1));
				portStream >> _listen;
				_host = _host.substr(0, pos);
			}
			break;
		}
	}
	// std::cout << _GOLD "Host is : " << _host << " on port " << _listen << _END << std::endl;

	// find matching server
	for (std::vector<Server *>::const_iterator i = this->getServerHandler()->getServers().begin(); i != this->_serverHandler->getServers().end(); ++i)
	{
		if (this->getCurrentServer() != NULL)
			break ;
		if ((*i)->getListen() == this->_listen)
		{
			for (std::vector<std::string>::const_iterator j = (*i)->getServerNames().begin(); j != (*i)->getServerNames().end(); ++j)
			{
				if (*j == _host)
				{
					this->setCurrentServer(*i);
					break ;
				}
			}
		}
	}

	// No matching servename found, using first server on the port
	if (this->getCurrentServer() == NULL)
	{
		for (std::vector<Server *>::const_iterator i = this->getServerHandler()->getServers().begin(); i != this->_serverHandler->getServers().end(); ++i)
			if ((*i)->getListen() == this->_listen) {
				this->setCurrentServer(*i);
				break ;
			}
	}

	findLocation();
	setMethodsRootIndex();
}

/*****************  CLASS METHODS *****************/

Response*	Request::newGet() {
	return ( new Get() );
}

Response*	Request::newPost() {
	return ( new Post() );
}

Response*	Request::newDelete() {
	return ( new Delete() );
}

void	Request::initResponse( Response* response )
{
	response->setEventSocket(this->_event_socket);
	response->setRequest(this->_request);
	response->setCurrentRequest(this);
	response->setResource(this->_resource);
	response->setEpfd(this->_epfd);
	response->setCurrentServer(this->_current_server);
	response->setLocation(this->_location);
	response->setRoot(this->_root);
	response->setIndex(this->_index);
	response->setMethods(this->_methods);
	response->setHost(this->_host);
	response->setListen(this->_listen);
}

void	Request::buildResponse( const uint16_t & status_code )
{
	if (_response)
		_response->responseError(status_code);
	else
	{
		_response = new Response;
		initResponse(_response);
		_response->responseError(status_code);
		setValread(0);
	}
}


void	Request::buildResponse()
{
	if (_response)
	{
		delete _response;
		_response = NULL;
	}

	if (_contentLength > _current_server->getClientMaxBodySize())
	{
		buildResponse(413);
		return ;
	}
	map_method	map_methods[N_METHODS + 1] = {
		{GET, "GET", &Request::newGet},
		{POST, "POST", &Request::newPost},
		{DELETE, "DELETE", &Request::newDelete},
		{0, "", NULL}
	};

	for ( int i = 0; i < N_METHODS; i++ ) {
		if ( _method == map_methods[i].s_method ) {
			if ( !(map_methods[i].method & _methods))
			{
				_response = new Response;
				initResponse(_response);
				_response->responseError(405);
				break ;
			}
			_response = (this->*map_methods[i].newMethod)();
			break ;
		}
	}

	if (_response)
	{
		initResponse(_response);
		_response->executeMethod();	
	}
	else
	{
		_response = new Response;
		initResponse(_response);
		std::cout << "Method : " << _method << std::endl;
		if (_response->getValread() != 0)
			_response->responseError(400);
	}
	if (_response->getValread() == 0)
		_valread = 0;
}


void	Request::determinism()
{
	struct epoll_event	modifiedList;

	modifiedList.data.ptr = this;

	if (_socketState == READ_STATE) {
		std::cout << _CYAN "READING Request on baby_socket " << this->_event_socket << _END << std::endl;
		this->setRequest();
		this->setAttributes();
		// std::cout << _PINK "Request : " << _request << _END << std::endl;
		// std::cout << _PINK "Resource : " << _resource << _END << std::endl;
		if (_finished == true || _contentLength > _current_server->getClientMaxBodySize() || _valread == 0)
		{
			_socketState = WRITE_STATE;
			modifiedList.events = EPOLLOUT;
		}
		else
		{
			_socketState = READ_STATE;
			modifiedList.events = EPOLLIN;
		}
	}
	else
	{
		std::cout << _AQUAMARINE "SENDING Response on baby_socket " << this->_event_socket << _END << std::endl;
		if ( isCGI( this->getResource()) ) {
			execCGIScript( this->getResource(), this->_event_socket);
			_valread = 0;
		}
		if (_method == "POST" && _finished == true)
			_valread = 0;
		buildResponse();
		_request = "";
		_socketState = READ_STATE;
		modifiedList.events = EPOLLIN;
	}

	if (_valread == 0) {
		std::cout << _BOLD _ORANGE "Connection closed on " << _event_socket << _END << std::endl;
		close(this->_event_socket);
		epoll_ctl(this->_epfd, EPOLL_CTL_DEL, _event_socket, NULL);
	}
	else if (epoll_ctl(this->_epfd, EPOLL_CTL_MOD, _event_socket, &modifiedList) == -1)
		std::cout << "EPOLL error" << std::endl;
}
   