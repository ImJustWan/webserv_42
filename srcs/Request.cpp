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
	_finished = false;
	_readBytes = 0;
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
std::string	const & Request::getRequest(void) const { return (this->_request); }
std::string	const & Request::getMethod(void) const { return (this->_method); }
std::string const & Request::getResource(void) const { return ( this->_resource ); }
Server* Request::getCurrentServer(void) const { return ( this->_current_server ); }


/* ****************  SETTERS **************** */

void	Request::setEpfd( const int epfd ) { this->_epfd = epfd; }
void	Request::setEventSocket( const int socket ) { this->_event_socket = socket; }
void	Request::setValread( const int valread ) { this->_valread = valread; }
void	Request::setSocketState( bool state ) { this->_socketState = state; }
void	Request::setCurrentServer( Server *current ) { this->_current_server = current; }
void	Request::setResource( std::string resource ) { this->_resource = resource; }
void	Request::setRequest( std::string request ) { this->_request = request; }

size_t Request::getContentLength( size_t const & found ) const
{
	const size_t	length = _request.find("\n", found) - found;
	const size_t	content_length = std::atoi(_request.substr(found, length).c_str());

	return ( content_length );
}

void	Request::setMethods( )
{
	std::map<std::string, Location *>	locations = this->_current_server->getLocations();
	
	std::string				resource = this->getResource();
	std::string 			root = this->_current_server->getRoot();
	
	root = root.substr(1);
	if (resource.substr(0, root.size()) == root)
		resource = resource.substr(root.size());
	
	size_t					size = resource.size();

	for (size_t i = 0; i < size; i++)
	{
		if (locations.find(resource) != locations.end())
		{
			_methods = locations[resource]->getMethods();
			return ;
		}
		resource.resize(resource.size() - 1);
	}
	_methods = this->_current_server->getMethods();
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

	_valread = read(this->_event_socket, buffer, sizeof(buffer) - 1);
	_readBytes += _valread;
	bytesConcat(_request, buffer, _valread);

	size_t headerEnd = _request.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
		_finished = true;

	size_t found = _request.find("Content-Length:");
	size_t content_length = (found != std::string::npos) ? getContentLength(found + 1 + std::strlen("Content-Length:")) : _valread;

	if (_readBytes < content_length)
		_finished = false;
	else
	{
		if (_method == "POST" || headerEnd != std::string::npos)
			_finished = true;
		_readBytes = 0;
	}
}

void	Request::setAttributes()
{
	std::istringstream	iss(Request::_request);

	iss >> _method >> _resource;
	// std::cout << "Method is now : " << _method << std::endl;
	std::cout << "Resource is now : " << _resource << std::endl;
	// set the methods for the current resource according to location or server
	setMethods();
	_resource.erase(0, 1);

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
	response->setResource(this->_resource);
	response->setEpfd(this->_epfd);
	response->setCurrentServer(this->_current_server);
}

void	Request::buildResponse()
{
	if (_response)
	{
		delete _response;
		_response = NULL;
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

		if ( _response->requestLineCheck() )
		{
			_response->executeMethod();
		}
	
	}
	else
	{
		_response = new Response;
		initResponse(_response);
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
		if (_finished == true)
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
