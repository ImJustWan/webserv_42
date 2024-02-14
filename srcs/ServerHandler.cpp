# include "ServerHandler.hpp"
# include "IEvent.hpp"
# include <signal.h>

extern sig_atomic_t signal_received;

ServerHandler::ServerHandler()
{
	constructorExtension(DEFAULT_CONF);
}

ServerHandler::ServerHandler(std::string const & filename)
{
	constructorExtension(filename);
}


void	ServerHandler::constructorExtension(std::string const & filename)
{
	fileChecker(filename);
	lexer(filename);
	checkBrackets(this->_tokens);
	checkForWhitelist();
	std::vector<std::vector<std::string > > serverData = ServerInfo(this->_tokens);
	std::cout << "\n❯❯❯ " _BOLD _EMERALD "Server Count " _END "[" << serverData.size() << "]\n" << std::endl;
	this->_servers.resize(serverData.size());
	for (size_t i = 0; i < serverData.size(); ++i) {
		this->_servers[i] = new Server(serverData[i], i + 1);
	}
	if (this->_servers.size() == 0)
		throw InvalidConfFileException();
	for (size_t i = 0; i < this->_servers.size(); ++i)
		std::cout << *(this->_servers[i]);
}

ServerHandler::~ServerHandler()
{
	for (std::vector<Server *>::iterator i = this->_servers.begin(); i != this->_servers.end(); ++i)
		delete *i;
	return;
}


std::vector<Server *> const & ServerHandler::getServers(void) const
{
	return this->_servers;
}

Server * ServerHandler::operator[](size_t id)
{
	if ( id >= this->_servers.size() )
		throw InvalidIndexException();
	return this->_servers[id];
}

void ServerHandler::fileChecker(std::string const &file)
{
	std::ifstream	infile(file.c_str());

	if(((infile.rdstate() | std::ifstream::goodbit) != 0) || infile.peek() == EOF)
		throw InvalidConfFileException();
	infile.close();
}

void ServerHandler::lexer(std::string const &file)
{
	std::vector<std::string>	tokens;
	std::ifstream				input(file.c_str());
	std::string					line;
	std::string 				word;

	while(getline(input, line))
	{
		size_t pos = line.find_first_not_of(" \t");
		if (pos != std::string::npos && line[pos] != '#')
		{
			if (line.find_first_of("#") != std::string::npos)
				line.erase(line.find('#'));
			std::stringstream 			ss(line);
			while (ss >> word) {
				while (word.find_first_of(DELIMITERS) != std::string::npos && word.size() != 1)
				{
					size_t delimPos = word.find_first_of(DELIMITERS);
					if (delimPos != 0) {
						std::string part1 = word.substr(0, delimPos);
						this->_tokens.push_back(part1);
						word = word.substr(delimPos, word.size());
					}
					else {
						std::string part1 = word.substr(0, 1);
						this->_tokens.push_back(part1);
						word = word.substr(delimPos + 1, word.size());
					}
				}
				this->_tokens.push_back(word);
			}
		}
	}
}

void ServerHandler::checkBrackets(std::vector<std::string> &rawData)
{
	int check = 0;
	for(std::vector<std::string>::iterator i = rawData.begin(); i != rawData.end(); ++i) {
		if (*i == "{")
			check++;
		if (*i == "}")
			check--;
	}
	if(check != 0)
		throw InvalidBlockException();
}
void ServerHandler::checkForWhitelist(void)
{
	std::string 			delimiters = "{};";

	std::set<std::string>	whitelist;
	whitelist.insert("http");
	whitelist.insert("listen");
	whitelist.insert("methods");
	whitelist.insert("root");
	whitelist.insert("index");
	whitelist.insert("client_max_body_size");
	whitelist.insert("server_name");
	whitelist.insert("location");
	whitelist.insert("error_page");
	whitelist.insert("autoindex");
	whitelist.insert("file_ext");
	whitelist.insert("cgi_path");
	whitelist.insert("autoindex");
	whitelist.insert("alias");
	whitelist.insert("server");
	whitelist.insert("}");



	for (std::vector<std::string>:: iterator i = this->_tokens.begin(); i != this->_tokens.end() -1; ++i)
	{
		if (delimiters.find(*i) != std::string::npos)
		{
			std::set<std::string>::iterator whitelistIT = whitelist.find(*(i + 1));
			if (whitelistIT == whitelist.end())
			{
				std::cout << "❯❯❯ " _BOLD _RED "Invalid Directive" _END " [" << *i << " " << *(i + 1) << "]" << std::endl;
				throw InvalidDirectiveException();
			}
		}
	}

	for (std::vector<std::string>:: iterator i = this->_tokens.begin() + 1; i != this->_tokens.end(); ++i)
	{
		std::set<std::string>::iterator whitelistIT = whitelist.find(*i);
		if (whitelistIT != whitelist.end() && delimiters.find(*(i - 1)) == std::string::npos)
		{
			std::cout << "❯❯❯ " _BOLD _RED "Invalid Directive" _END " [" << *i << " " << *(i + 1) << "]" << std::endl;
			throw InvalidDirectiveException();
		}
	}
}

std::vector<std::vector<std::string > > ServerHandler::ServerInfo(std::vector<std::string> &rawData)
{
	int serverCount = 0;
	std::vector<std::vector<std::string > >	serverinfo;
	for(std::vector<std::string>::iterator i = rawData.begin(); i != rawData.end(); ++i)
	{
		int bracket = 0;
		if (i != rawData.end() && *i == "server" && *(++i) == "{"){
			serverinfo.resize(serverCount + 1);
			for (std::vector<std::string>::iterator j = i; j != rawData.end(); ++j)
			{
				serverinfo[serverCount].push_back(*j);
				if (*j == "{")
					bracket++;
				if (*j == "}")
					bracket--;
				if (bracket == 0)
					break;
			}
			serverCount++;
		}
	}
	return serverinfo;
}

void	ServerHandler::addMasterSockets()
{
	struct epoll_event interestList;

	interestList.events = EPOLLIN;
	for (std::vector<Server *>::iterator it	= this->_servers.begin(); it < this->_servers.end(); it++)
	{
		Server *current = *it;
		current->buildSocket();
		interestList.data.ptr = current;
		current->setEpfd(this->_epfd);
		if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, current->getMasterSocket(), &interestList) == -1)
			std::cout << "MASTER epoll error" << std::endl;

	}
}

void	signalHandler(int signum)
{
	signal_received = true;
	(void)signum;
}


void	ServerHandler::handleServers()
{
	this->_epfd = epoll_create(1);
	addMasterSockets();

	int readyz;
	signal(SIGINT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	while (signal_received == false)
	{
		readyz = epoll_wait(this->_epfd, this->_readyList, 50, 10000); // max fds = 1024 so lower
		if (readyz != 0)
		{
			for (int i = 0; i < readyz; i++) {
				reinterpret_cast<IEvent *>(this->_readyList[i].data.ptr)->determinism();
			}
		}
		else
			std::cout << "No events :(" << std::endl;
	}
}
