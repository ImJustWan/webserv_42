	void	closeSocket(int socket);
#include "Server.hpp"

/*****************  CANONICAL FORM *****************/

/*****************  CANONICAL FORM *****************/


Server::Server(std::vector<std::string> &tokens, int id) : _serverID(id), _listen(0), _autoindex(true), _methods(0), _clientMaxBodySize(1000000), _master_socket(-1),_request_index(0)
{
	configurationMap();
	for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i) {
		std::map<std::string, void (Server::*)(std::vector<std::string>)>::iterator configIt = this->_configMap.find(*i);
		if (configIt != this->_configMap.end() && configIt->first == "location")
		{
			directiveIsolation("}", tokens, configIt, i);
			while (*i != "}")
				++i;
		}
		else if (configIt != this->_configMap.end())
			directiveIsolation(";", tokens, configIt, i);
	}
	checkMinimumConf();
	for (int i = 0; i < MAX_REQUEST; ++i)
		_requests[i] = NULL;
}

Server::~Server()
{
	for (std::map<std::string, Location *>::iterator i = this->_currentLocations.begin(); i != this->_currentLocations.end(); ++i)
		delete i->second;
	if (this->_master_socket != -1)
		close(this->_master_socket);
	for (int i = 0; i < MAX_REQUEST; ++i) {
		if (_requests[i] != NULL)
		{
			_requests[i]->setEventSocket(closeSocket(_requests[i]->getEventSocket()));
			delete _requests[i];
			_requests[i] = NULL;
		}
	}
}

/*****************  CLASS METHODS *****************/

void Server::configurationMap(void)
{
	this->_configMap["listen"] = &Server::setListen;
	this->_configMap["methods"] = &Server::setMethods;
	this->_configMap["root"] = &Server::setRoot;
	this->_configMap["index"] = &Server::setIndex;
	this->_configMap["client_max_body_size"] = &Server::setClientMaxBodySize;
	this->_configMap["server_name"] = &Server::setServerNames;
	this->_configMap["location"] = &Server::setLocations;
	this->_configMap["error_page"] = &Server::setErrors;
	this->_configMap["autoindex"] = &Server::setAutoindex;
}

void Server::directiveIsolation(std::string delim, std::vector<std::string> &tokens,
std::map<std::string, void (Server::*)(std::vector<std::string>)>::iterator configIt,
std::vector<std::string>::iterator i)
{
	std::vector<std::string> directive;
	while (i != tokens.end() && *i != delim) {
		directive.push_back(*i);
		++i;
	}
	if (directive.size() < 2){
		std::cout << directive[0] << std::endl;
		throw InvalidConfig(INVALCONF "Incomplete Directive");
	}
	if (delim == "}")
		directive.push_back(*i);
	(this->*configIt->second)(directive);
}

void Server::earlyDeath(void)
{
	for (std::map<std::string, Location *>::iterator i = this->_currentLocations.begin(); i != this->_currentLocations.end(); ++i)
		delete i->second;
}

void Server::checkMinimumConf(void)
{
	if (this->_listen == 0){
		earlyDeath();
		throw InvalidConfig(INVALCONF "Incomplete configuration");
	}
	if (this->_root.empty()){
		earlyDeath();
		throw InvalidConfig(INVALCONF "Missing root folder");
	}
	if (this->_index.empty()){
		earlyDeath();
		throw InvalidConfig(INVALCONF "Missing index file");
	}
	std::string	file = this->_root + "/" + this->_index;
	std::ifstream	infile(file.c_str());
	if(((infile.rdstate() | std::ifstream::goodbit) != 0) || infile.peek() == EOF){
		earlyDeath();
		throw InvalidConfig(INVALCONF "Invalid root or index file");
	}
	infile.close();
}

/* ****************  GETTERS **************** */

int Server::getServerID(void) const { return this->_serverID; }
uint16_t const & Server::getListen(void) const { return this->_listen; }
int const & Server::getMethods(void) const { return this->_methods; }
std::string const & Server::getRoot(void) const { return this->_root; }
std::string const & Server::getIndex(void) const { return this->_index; }
std::string const & Server::getUploadPath(void) const { return this->_uploadPath; }
bool const & Server::getAutoindex(void) const { return this->_autoindex; }
long long int const & Server::getClientMaxBodySize(void) const { return this->_clientMaxBodySize; }
std::vector<std::string> const & Server::getServerNames(void) const { return this->_serverNames; }
std::map<std::string, Location *> const & Server::getLocations(void) const { return this->_currentLocations; }
std::map<uint16_t, std::string>	const & Server::getErrors(void) const { return this->_errors; }
ServerHandler * Server::getServerHandler(void) const { return this->_currentServerHandler; }

int const & Server::getEpfd(void) const { return this->_epfd; }
int const & Server::getReqIndex(void) const { return this->_request_index; }
int const & Server::getMasterSocket(void) const { return this->_master_socket; }
int const & Server::getBabySocket(size_t idx) const { return this->_baby_socket[idx]; }

Request * Server::getRequest(int index) { if (index <= this->_request_index) return this->_requests[index]; return NULL; }


/* ****************  PARSING SETTERS **************** */

void Server::setListen(std::vector<std::string> listen)
{
	uint16_t	data;
	data = dataExtractor<uint16_t>(listen[1]);
	if (data < 8000 || data > 8999)
		throw InvalidConfig(INVALCONF "Listen Directive");
	this->_listen = data;
}

void Server::setMethods(std::vector<std::string> methods)
{
	std::string methodsID[5] = {"", "GET", "POST", "", "DELETE"};
	for(std::vector<std::string>::iterator i = methods.begin() + 1; i!= methods.end(); ++i)
	{
		size_t j = 1;
		for(; j < 5 ; ++j)
			if (*i == methodsID[j]){
				this->_methods |= j;
				break ;
			}
		if (j == 5)
			throw InvalidConfig(INVALCONF "Unknown method");
	}
}

void Server::setRoot(std::vector<std::string> root)
{
	std::string	data;
	data = dataExtractor<std::string>(root[1]);
	std::ifstream test(data.c_str());
	if (!test)
		throw InvalidConfig(INVALCONF "Root Directive");
	test.close();
	this->_root = data;
}

void Server::setIndex(std::vector<std::string> index)
{
	std::string	data;
	data = dataExtractor<std::string>(index[1]);
	if (data.rfind(".html\0") == std::string::npos)
		throw InvalidConfig(INVALCONF "Index Directive");
	this->_index = data;
}


void Server::setAutoindex(std::vector<std::string> autoindex)
{
	if (autoindex[1] == "on")
		this->_autoindex = true;
	else if (autoindex[1] == "off")
		this->_autoindex = false;
	else
		throw InvalidConfig(INVALCONF "Autoindex Directive");
}

void Server::setClientMaxBodySize(std::vector<std::string> clientMaxBodySize)
{

	long long unsigned int	data;
	data = dataExtractor<long long unsigned int>(clientMaxBodySize[1]);
	if (data > 10000000000)
		throw InvalidConfig(INVALCONF "Client Max Body Size Directive");
	this->_clientMaxBodySize = data;
}

void Server::setServerNames(std::vector<std::string> serverNames)
{

	size_t	size = serverNames.size() - 1;
	if (size == 0)
		throw InvalidConfig(INVALCONF "Server Name Directive");
	for (std::vector<std::string>::iterator i = serverNames.begin() + 1; i != serverNames.end(); ++i)
		this->_serverNames.push_back(*i);
}

void Server::setLocations(std::vector<std::string> locations)
{
	if (this->_currentLocations.find(locations[1]) != this->_currentLocations.end()){
		for (std::map<std::string, Location *>::iterator i = this->_currentLocations.begin(); i != this->_currentLocations.end(); ++i) {
			delete i->second;
		}
		throw InvalidConfig(INVALCONF "Duplicate Location Directive");
	}
	try
	{
		Location    * newLocation = new Location(locations, this);
		this->_currentLocations[newLocation->getDirectory()] = newLocation;
		if (newLocation->getMethods() == 0)
			newLocation->setMethods(this->_methods);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		for (std::map<std::string, Location *>::iterator i = this->_currentLocations.begin(); i != this->_currentLocations.end(); ++i)
			delete i->second;
		throw InvalidConfig(INVALCONF "error in location parsing");
	}
	
}

void Server::setErrors(std::vector<std::string> errors)
{
	if (errors.size() != 3)
		throw InvalidConfig(INVALCONF "Errors Directive");
	uint16_t code = dataExtractor<uint16_t>(errors[1]);
	if (code < 400  || code > 599 || this->_errors.find(code) != this->_errors.end())
		throw InvalidConfig(INVALCONF "Error page code");
	if (errors[2].rfind(".html\0") == std::string::npos)
		throw InvalidConfig(INVALCONF "Errors page file extension");
	this->_errors[code] = errors[2];
}



/* **************** EXEC SETTERS **************** */


void Server::setServerHandler(ServerHandler *serverHandler) { this->_currentServerHandler = serverHandler; }
void Server::setMasterSocket(int masterSocket) { this->_master_socket = masterSocket; }
void Server::setEpfd(const int epfd) { this->_epfd = epfd; }
void Server::setUploadPath(std::string path) { this->_uploadPath = path; }


void Server::eraseRequest(int index)
{
	if (index < _request_index)
	{
		if (_requests[index] != NULL)
		{
			_requests[index]->setEventSocket(closeSocket(_requests[index]->getEventSocket()));
			delete _requests[index];
			_requests[index] = NULL;
			_request_index--;
		}
	}
}

/* ****************  CLASS METHODS **************** */

void	Server::buildSocket( void ) {
	this->ling.l_onoff = 1;
	this->ling.l_linger = 0;
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons( this->_listen );
	addrlen = sizeof(address);
	this->_request_index = 0;
	this->_request_index = 0;
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));

	if ((this->_master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw SocketError();
	errno = 0;
	if (fcntl(_master_socket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
	if (setsockopt(this->_master_socket, SOL_SOCKET, SO_REUSEADDR, &ling, sizeof(ling)) < 0)
		throw SocketOptionError();
	if (bind(this->_master_socket, (struct sockaddr *)&address, sizeof(address))<0)
		throw BindError();
	if (listen(this->_master_socket, 10) < 0)
		throw ListenError();
}

int	Server::closeSocket(int socket)
{
	if (socket == -1)
		return (-1);
	close(socket);
	if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, socket, NULL) == -1)
		;
	return (-1);
}

void	Server::initRequest( Request* request )
{
	request->setSocketState(READ_STATE);
	request->setEventSocket(this->_baby_socket[_request_index]);
	request->setEpfd(this->_epfd);
	request->setServerHandler(this->_currentServerHandler);
	request->setCurrentServer(NULL);
	request->setLocation(NULL);
	request->setLastEvent();
}

void Server::serverProcess() {
	struct epoll_event interestList;

	interestList.events = EPOLLIN;

	if (this->_request_index == MAX_BBY_SOCKET)
		return ;
	if ((this->_baby_socket[_request_index] = accept(this->_master_socket, (struct sockaddr *)&address, &addrlen)) < 0)
		throw SocketLoopError();

	Request *current = new Request;

	initRequest( current );
	interestList.data.ptr = current;
	if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, current->getEventSocket(), &interestList) == -1)
		std::cout << "MASTER epoll error" << std::endl;
	for (int i = 0; i < MAX_REQUEST; ++i)
	{
		if (_requests[i] == NULL)
		{
			_requests[i] = current;
			_request_index++;
			break ;
		}
	}
}

void	Server::determinism()
{
	// std::cout << _SALMON "Creating new baby_socket" _END << std::endl;
	serverProcess();
}

