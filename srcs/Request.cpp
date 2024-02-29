# include "Request.hpp"
# include "Response.hpp"
# include "Get.hpp"
# include "Post.hpp"
# include "Delete.hpp"
# include "CgiHandler.hpp"

/*****************  CANONICAL FORM *****************/


Request::Request() : 
	_currentLocation(NULL),
	_currentResponse(NULL),
	_currentCGI(NULL),
	_epfd(0),
	_event_socket(0),
	_socketState(READ_STATE),
	_request(""),
	_method(""),
	_resource(""),
	_cgiExt(""),
	_readBytes(0),
	_chunked(false),
	_readFinished(false),
	_sentFinished(false),
	_responseReady(true),
	_readLength(0),
	_contentLength(0) {
}

Request::~Request() {
	if (_currentResponse)
		delete _currentResponse;
	if (_currentCGI)
		delete _currentCGI;
}

Request::Request ( const Request& src ) : IEvent(src) {
	*this = src;
}

Request& Request::operator= ( const Request& cpy ) {
	if (this == &cpy)
		return (*this);
	this->_currentServerHandler = cpy._currentServerHandler;
	this->_currentServer = cpy._currentServer;
	this->_currentLocation = cpy._currentLocation;
	this->_currentResponse = cpy._currentResponse;

	this->_epfd = cpy._epfd;
	this->_event_socket = cpy._event_socket;
	this->_socketState = cpy._socketState;

	this->_index = cpy._index;
	this->_root = cpy._root;
	this->_host = cpy._host;
	this->_request = cpy._request;
	this->_method = cpy._method;
	this->_resource = cpy._resource;
	this->_cgiExt = cpy._cgiExt;

	this->_methods = cpy._methods;
	this->_listen = cpy._listen;

	this->_readBytes = cpy._readBytes;
	this->_readFinished = cpy._readFinished;
	this->_sentFinished = cpy._sentFinished;
	this->_readLength = cpy._readLength;
	this->_contentLength = cpy._contentLength;
	this->_lastEvent = cpy._lastEvent;
	return *this;
}


/* ****************  GETTERS **************** */

int const &	Request::getEpfd(void) const { return (this->_epfd); }
int const &	Request::getReadBytes(void) const { return (this->_readBytes); }
int const &	Request::getEventSocket(void) const { return (this->_event_socket); }
int const &	Request::getMethods(void) const { return (this->_methods); }
int const &	Request::getListen(void) const { return (this->_listen); }
bool		Request::getReady(void) const { return (this->_responseReady); }
bool		Request::getReadFinished(void) const { return (this->_readFinished); }
bool		Request::getSentFinished(void) const { return (this->_sentFinished); }
bool		Request::getChunked(void) const { return (this->_chunked); }
std::string	const & Request::getRequest(void) const { return (this->_request); }
std::string	const & Request::getMethod(void) const { return (this->_method); }
std::string const & Request::getResource(void) const { return ( this->_resource ); }
std::string	const & Request::getHTTP(void) const { return (this->_http); }
Server* Request::getCurrentServer(void) const { return ( this->_currentServer ); }
Location* Request::getLocation() const { return ( this->_currentLocation ); }
Response* Request::getResponse() const { return ( this->_currentResponse ); }
ServerHandler* Request::getServerHandler() const { return ( this->_currentServerHandler ); }
std::string const & Request::getIndex(void) const { return ( this->_index ); }
std::string const & Request::getHost(void) const { return ( this->_host ); }
std::string const & Request::getRoot(void) const { return ( this->_root ); }
std::string	const & Request::getFileExt(void) const { return ( this->_fileExt); }


/* ****************  SETTERS **************** */

void	Request::setEpfd( const int epfd ) { this->_epfd = epfd; }
void	Request::setMethods( const int methods ) { this->_methods = methods; }
void	Request::setListen( const int listen ) { this->_listen = listen; }
void	Request::setEventSocket( const int socket ) { this->_event_socket = socket; }
void	Request::setReadBytes( const int readbytes ) { this->_readBytes = readbytes; }
void	Request::setSocketState( bool state ) { this->_socketState = state; }
void	Request::setCurrentServer( Server *current ) { this->_currentServer = current; }
void	Request::setLocation( Location *location ) { this->_currentLocation = location; }
void	Request::setResponse( Response *response ) { this->_currentResponse = response; }
void	Request::setServerHandler( ServerHandler *serverHandler ) { this->_currentServerHandler = serverHandler; }
void	Request::setRoot( std::string root ) { this->_root = root; }
void	Request::setIndex( std::string index ) { this->_index = index; }
void	Request::setHost( std::string host ) { this->_host = host; }
void	Request::setResource( std::string resource ) { this->_resource = resource; }
void	Request::setRequest( std::string request ) { this->_request = request; }
void	Request::setFinalResponse( std::string finalResponse ) { this->_finalResponse = finalResponse; }

void	Request::setLastEvent( void ) { this->_lastEvent = time(NULL); }
void 	Request::setLastEvent(long long int time) { this->_lastEvent = time; }
void	Request::setAsReady(bool state) { this->_responseReady = state; }
void	Request::setReadFinished(bool state) { this->_readFinished = state; }
void	Request::setSentFinished(bool state) { this->_sentFinished = state; }


bool	Request::checkTimeout()
{
	// std::cout << "Diff is : " << time(NULL) - this->_lastEvent << std::endl;
	if (time(NULL) - this->_lastEvent > TIMEOUT)
		return false;
	return true;
}


size_t Request::findContentLength( size_t const & found ) const
{
	const size_t	length = _request.find("\n", found) - found;
	if (length == std::string::npos)
		return ( 0 );
	const size_t	content_length = std::atoi(_request.substr(found, length).c_str());
 
	return ( content_length );
}

void	Request::setLocation()
{
	if (this->_currentServer == NULL)
		return ;
	if (this->_currentServer->getLocations().empty())
		return ;
	std::map<std::string, Location *>	locations = this->_currentServer->getLocations();
	
	std::string				resource = this->getResource();
	std::string 			root = "/";
	
	root += this->_currentServer->getRoot();
	
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

void Request::handleHeader() {
	size_t headerEnd = _request.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
	{
		if ( _request.find("POST") == std::string::npos)
			_readFinished = true;
		else
		{
			_readFinished = false;
			size_t found = _request.find("Content-Length:");
			if (found != std::string::npos)
				_contentLength = findContentLength(found + 1 + std::strlen("Content-Length:"));
			if (found == std::string::npos || _contentLength == 0)
				_chunked = true;
		}
	}
}


void Request::setRequest() {
	char buffer[READ_BUFFER_SIZE] = "";

	_readBytes = recv(this->_event_socket, buffer, sizeof(buffer) - 1, 0);
	
	std::string tmp = buffer;
	if (_readBytes <= 0) // if 0, handled in determinism
	{
		if (_readBytes == -1)
			this->_lastEvent = 0; // will delete client/socket in main loop
		return ;
	}

	_request.append(buffer, _readBytes);
	_readLength += _readBytes;

	size_t headerEnd = _request.find("\r\n\r\n");
	handleHeader();
	if (_readLength < _contentLength)
		_readFinished = false;
	else
	{
		if (_method == "POST" || headerEnd != std::string::npos)
			_readFinished = true;
		_readLength = 0;
	}

	if (_chunked == true)
		_readFinished = false;
	if (_chunked == true && _request.find("\r\n0\r\n\r\n") != std::string::npos)
		_readFinished = true;
}


void	Request::setMethodsRootIndex()
{

	if (this->getLocation() != NULL)
		this->setMethods(this->getLocation()->getMethods());
	else
		this->setMethods(this->getCurrentServer()->getMethods());

	if (this->getLocation() != NULL && this->getLocation()->getRoot() != "")
		this->setRoot(this->getLocation()->getRoot());
	else if (this->getLocation() != NULL || this->getCurrentServer() == NULL)
		this->setRoot("");
	else
		this->setRoot(this->getCurrentServer()->getRoot());

	if (this->getLocation() != NULL && this->getLocation()->getIndex() != "")
		this->setIndex(this->getLocation()->getIndex());
	else if (this->getLocation() != NULL || this->getCurrentServer() == NULL)
		this->setIndex("");
	else
		this->setIndex(this->getCurrentServer()->getIndex());
}

void	Request::setAttributes()
{
	std::istringstream	iss(Request::_request);

	iss >> _method >> _resource >> _http;

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
	for (std::vector<Server *>::const_iterator i = this->getServerHandler()->getServers().begin(); i != this->_currentServerHandler->getServers().end(); ++i)
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

	// No matching servername found, using first server on the port
	if (this->getCurrentServer() == NULL)
	{
		for (std::vector<Server *>::const_iterator i = this->getServerHandler()->getServers().begin(); i != this->_currentServerHandler->getServers().end(); ++i)
			if ((*i)->getListen() == this->_listen) {
				this->setCurrentServer(*i);
				break ;
			}
	}
	if (this->getCurrentServer() == NULL)
		this->setCurrentServer(this->getServerHandler()->getServers().front());
	setLocation();
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

void	Request::buildErrorResponse( const uint16_t & status_code )
{
	if (_currentResponse)
		_currentResponse->responseError(status_code);
	else
	{
		_currentResponse = new Response;
		_currentResponse->setCurrentRequest(this);
		_currentResponse->responseError(status_code);
		setReadBytes(0);
	}
}

bool	Request::checkMethodsBlacklist()
{
	std::set<std::string>	blackList;
	blackList.insert("HEAD");
	blackList.insert("PUT");
	blackList.insert("CONNECT");
	blackList.insert("OPTIONS");
	blackList.insert("TRACE");
	blackList.insert("PATCH");

	if (blackList.find(this->_method) != blackList.end())
		return false;
	return true;
}


void	Request::buildResponse()
{
	if (_currentResponse && _responseReady == false)
	{
		_currentResponse->executeMethod();
		return ;
	}
	if (_currentResponse)
	{
		delete _currentResponse;
		_currentResponse = NULL;
	}

	if (_contentLength > _currentServer->getClientMaxBodySize())
		throw ResponseBuildingError(413);

	if (checkMethodsBlacklist() == false) 
		throw ResponseBuildingError(501);

	map_method	map_methods[N_METHODS + 1] = {
		{GET, "GET", &Request::newGet},
		{POST, "POST", &Request::newPost},
		{DELETE, "DELETE", &Request::newDelete},
		{0, "", NULL}
	};

	for ( int i = 0; i < N_METHODS; i++ ) {
		if ( _method == map_methods[i].s_method ) {
			if (_method == "POST" && this->getLocation() && this->getLocation()->getUploadPath().empty() == true)
					throw ResponseBuildingError(501);
			if ( !(map_methods[i].method & _methods))
					throw ResponseBuildingError(405);
			_currentResponse = (this->*map_methods[i].newMethod)();
			break ;
		}
	}

	if (_currentResponse)
	{
		_currentResponse->setCurrentRequest(this);
		_currentResponse->executeMethod();	
	}
	else
		throw ResponseBuildingError(400);
}

bool	Request::isCGI(std::string const & resource)
{
	size_t		dot;
	std::string	fileExt;
	
	if (this->_method == "DELETE")
		return false;

	else if (this->_method == "POST"){
		dot = resource.rfind('.');
		if (dot == std::string::npos)
			return false;
		fileExt = resource.substr(dot, std::string::npos);
		this->_fileExt = fileExt;
	}

	else if (this->_method == "GET"){
		size_t		query;
		query = resource.rfind('?');
		fileExt = resource.substr(0, query);
		dot = fileExt.rfind('.');
		if (dot == std::string::npos)
			return false;
		fileExt = fileExt.substr(dot, std::string::npos);
		this->_fileExt = fileExt;
	}
	if (this->_currentLocation && this->_currentLocation->getFileExt().size() != 0 && this->_currentLocation->getFileExt() == fileExt){
		this->_cgiExt = fileExt;
		return true;
	}
	return false;
}

void	Request::changeSocketState()
{
	struct epoll_event	modifiedList;

	modifiedList.data.ptr = this;
	if (_socketState == READ_STATE)
		modifiedList.events = EPOLLIN;
	else
		modifiedList.events = EPOLLOUT;
	if (epoll_ctl(this->_epfd, EPOLL_CTL_MOD, _event_socket, &modifiedList) == -1)
	{
		_event_socket = this->getCurrentServer()->closeSocket(_event_socket);
		this->_lastEvent = 0; // will delete client/socket in main loop
	}
}

void	Request::HandleCGI()
{
	if (this->_currentCGI == NULL){
		_responseReady = false;
		_currentCGI = new CgiHandler(this);
		_socketState = WRITE_STATE;
	}
	else {
		if (_responseReady == false && _currentCGI->getCgiStatus() == 4) {
			_socketState = READ_STATE;
			_currentCGI->sendResponse();
			_responseReady = true;
		}
		else
		{
			if (_currentCGI->getCgiStatus() < 4) {
				_currentCGI->execCGI();
				_socketState = WRITE_STATE;
			}
			else if (_currentCGI->getCgiStatus() == 4) {
				_responseReady = false;
			}
			if (_currentCGI->getCgiStatus() == 5)
				_responseReady = true;
		}
	}
}

void	Request::determinism()
{
	this->setLastEvent();
	if (_socketState == READ_STATE) {
		std::cout << _CYAN "EPOLLIN on baby_socket " << this->_event_socket << _END << std::endl;
		this->setRequest();
		this->setAttributes();
		// std::cout << _PINK "Request : " << _request << _END << std::endl;
		// std::cout << _PINK _REV "Resource : " << _resource << _END << std::endl;
		if (_readFinished == true || _contentLength > getCurrentServer()->getClientMaxBodySize())
			_socketState = WRITE_STATE;
		else
			_socketState = READ_STATE;
	}
	else
	{
		std::cout << _AQUAMARINE "EPOLLOUT on baby_socket " << this->_event_socket << _END << std::endl;
		if ( isCGI( this->getResource()) ) {
			HandleCGI();
		}
		else
		{
			try {
				buildResponse();
				if (_method == "POST" && _readFinished == true)
					_readBytes = 0;
				if (_sentFinished == false)
					_socketState = WRITE_STATE;
			}
			catch (ResponseBuildingError &e) {
				buildErrorResponse(e.getErrorCode());
			}
		}

		if (_responseReady == true)
		{
			std::cout << _EMERALD "Response is ready" << _END << std::endl;
			// std::cout << _PINK << this->_finalResponse << _END << std::endl;
			int sent = send(this->_event_socket, this->_finalResponse.c_str(), this->_finalResponse.size(), 0);
			if (sent == -1)
			{
				std::cout << _RED "Error sending response" << _END << std::endl;
				this->_lastEvent = 0; // will delete client/socket in main loop
			}
			else if (sent == 0)
				std::cout << _GREY _ITALIC "Sent response was empty" _END << std::endl;
			_socketState = READ_STATE;
			_request = "";
		}
	}

	changeSocketState();
	if (_readBytes <= 0)
	{
		_event_socket = this->getCurrentServer()->closeSocket(_event_socket);
		this->_lastEvent = 0; // will delete client/socket in main loop
	}
}