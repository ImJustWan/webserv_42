# include "ServerHandler.hpp"
# include "IEvent.hpp"
# include <signal.h>

extern sig_atomic_t signal_received;

/*****************  CANONICAL FORM *****************/

ServerHandler::ServerHandler()
{
	constructorExtension(DEFAULT_CONF);
}

ServerHandler::ServerHandler(std::string const & filename)
{
	constructorExtension(filename);
}

std::map<std::string, std::string> const & ServerHandler::getMimeMap(void) const { return this->_mimeMap; }

void	ServerHandler::createMimeMap()
{
	_mimeMap[".aac"] = "audio/aac";
	_mimeMap[".abw"] = "application/x-abiword";
	_mimeMap[".arc"] = "application/x-freearc";
	_mimeMap[".avif"] = "image/avif";
	_mimeMap[".avi"] = "video/x-msvideo";
	_mimeMap[".azw"] = "application/vnd.amazon.ebook";
	_mimeMap[".bin"] = "application/octet-stream";
	_mimeMap[".bmp"] = "image/bmp";
	_mimeMap[".bz"] = "application/x-bzip";
	_mimeMap[".bz2"] = "application/x-bzip2";
	_mimeMap[".cda"] = "application/x-cdf";
	_mimeMap[".csh"] = "application/x-csh";
	_mimeMap[".css"] = "text/css";
	_mimeMap[".csv"] = "text/csv";
	_mimeMap[".doc"] = "application/msword";
	_mimeMap[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mimeMap[".eot"] = "application/vnd.ms-fontobject";
	_mimeMap[".epub"] = "application/epub+zip";
	_mimeMap[".gz"] = "application/gzip";
	_mimeMap[".gif"] = "image/gif";
	_mimeMap[".html"] = "text/html";
	_mimeMap[".htm"] = "text/html";
	_mimeMap[".ico"] = "image/vnd.microsoft.icon";
	_mimeMap[".ics"] = "text/calendar";
	_mimeMap[".jar"] = "application/java-archive";
	_mimeMap[".jpg"] = "image/jpeg";
	_mimeMap[".jpeg"] = "image/jpeg";
	_mimeMap[".js"] = "text/javascript";
	_mimeMap[".json"] = "application/json";
	_mimeMap[".jsonld"] = "application/ld+json";
	_mimeMap[".midi"] = "audio/x-midi";
	_mimeMap[".midi"] = "audio/midi";
	_mimeMap[".mid"] = "audio/x-midi";
	_mimeMap[".mid"] = "audio/midi";
	_mimeMap[".mjs"] = "text/javascript";
	_mimeMap[".mp3"] = "audio/mpeg";
	_mimeMap[".mp4"] = "video/mp4";
	_mimeMap[".mpeg"] = "video/mpeg";
	_mimeMap[".mpkg"] = "application/vnd.apple.installer+xml";
	_mimeMap[".odp"] = "application/vnd.oasis.opendocument.presentation";
	_mimeMap[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_mimeMap[".odt"] = "application/vnd.oasis.opendocument.text";
	_mimeMap[".oga"] = "audio/ogg";
	_mimeMap[".ogv"] = "video/ogg";
	_mimeMap[".ogx"] = "application/ogg";
	_mimeMap[".opus"] = "audio/opus";
	_mimeMap[".otf"] = "font/otf";
	_mimeMap[".png"] = "image/png";
	_mimeMap[".pdf"] = "application/pdf";
	_mimeMap[".php"] = "application/x-httpd-php";
	_mimeMap[".ppt"] = "application/vnd.ms-powerpoint";
	_mimeMap[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mimeMap[".rar"] = "application/vnd.rar";
	_mimeMap[".rtf"] = "application/rtf";
	_mimeMap[".sh"] = "application/x-sh";
	_mimeMap[".svg"] = "image/svg+xml";
	_mimeMap[".tar"] = "application/x-tar";
	_mimeMap[".tiff"] = "image/tiff";
	_mimeMap[".tif"] = "image/tiff";
	_mimeMap[".ts"] = "video/mp2t";
	_mimeMap[".ttf"] = "font/ttf";
	_mimeMap[".txt"] = "text/plain";
	_mimeMap[".vsd"] = "application/vnd.visio";
	_mimeMap[".wav"] = "audio/wav";
	_mimeMap[".weba"] = "audio/webm";
	_mimeMap[".webm"] = "video/webm";
	_mimeMap[".webp"] = "image/webp";
	_mimeMap[".woff"] = "font/woff";
	_mimeMap[".woff2"] = "font/woff2";
	_mimeMap[".xhtml"] = "application/xhtml+xml";
	_mimeMap[".xls"] = "application/vnd.ms-excel";
	_mimeMap[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mimeMap[".xml"] = "application/xml";
	_mimeMap[".xul"] = "application/vnd.mozilla.xul+xml";
	_mimeMap[".zip"] = "application/zip";
	_mimeMap[".3gp"] = "video/3gpp";
	_mimeMap[".3g2"] = "video/3gpp2";
	_mimeMap[".7z"] = "application/x-7z-compressed";
	_mimeMap[".pl"] = "application/x-perl";


}

void	ServerHandler::constructorExtension(std::string const & filename)
{
	createMimeMap();
	fileChecker(filename);
	lexer(filename);
	checkBrackets(this->_tokens);
	checkForWhitelist();
	std::vector<std::vector<std::string > > serverData = ServerInfo(this->_tokens);
	std::cout << "\n❯❯❯ " _BOLD _EMERALD "Server Count " _END "[" << serverData.size() << "]\n" << std::endl;
	this->_servers.resize(serverData.size());
	for (size_t i = 0; i < serverData.size(); ++i) {
		this->_servers[i] = new Server(serverData[i], i + 1);
		this->_servers[i]->setServerHandler(this);
	}
	if (this->_servers.size() == 0)
		throw InvalidConfFileException();
	// for (size_t i = 0; i < this->_servers.size(); ++i)
	// 	std::cout << *(this->_servers[i]);
}

ServerHandler::~ServerHandler()
{
	close(this->_epfd);
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
	if (this->_tokens.size() == 0)
		throw InvalidConfFileException();
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
	whitelist.insert("upload_path");
	whitelist.insert("}");



	for (std::vector<std::string>:: iterator i = this->_tokens.begin(); i != this->_tokens.end() -1; ++i)
	{
		if (delimiters.find(*i) != std::string::npos)
		{
			std::set<std::string>::iterator whitelistIT = whitelist.find(*(i + 1));
			if (whitelistIT == whitelist.end())
			{
				std::cout << "❯❯❯ " _BOLD _RED "Invalid Directive" _END " [" << *i << ", " << *(i + 1) << "]" << std::endl;
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
	std::map<int, Server *> serverMap;

	interestList.events = EPOLLIN;
	for (std::vector<Server *>::iterator it	= this->_servers.begin(); it < this->_servers.end(); it++)
	{
		Server *current = *it;
		current->setEpfd(this->_epfd);
		if (serverMap.find(current->getListen()) == serverMap.end())
		{
			current->buildSocket();
			serverMap[current->getListen()] = current;
			std::cout << "New Server on port " << current->getListen() << std::endl;
			interestList.data.ptr = current;
			if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, current->getMasterSocket(), &interestList) == -1)
				std::cout << "MASTER epoll error" << std::endl;
		}
		else
		{
			current->setMasterSocket(serverMap[current->getListen()]->getMasterSocket());
			std::cout << "Server already exists on port " << current->getListen() << std::endl;
		}
	}
}

void	signalHandler(int signum)
{
	signal_received = true;
	(void)signum;
}


void	ServerHandler::handleServers()
{
	if ((this->_epfd = epoll_create(1)) == -1)
		throw EpollException();
	addMasterSockets();

	int readyz;
	signal(SIGINT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	while (signal_received == false)
	{
		readyz = epoll_wait(this->_epfd, this->_readyList, 100, 5000); // max fds = 1024 so lower
		if (readyz != 0)
		{
			for (int i = 0; i < readyz; i++) {
				reinterpret_cast<IEvent *>(this->_readyList[i].data.ptr)->determinism();
			}
		}
		else
			std::cout << "No events :(" << std::endl;
		
		for (std::vector<Server *>::reverse_iterator it = this->_servers.rbegin(); it != this->_servers.rend(); it++) {
			for (int i = 0; i < MAX_REQUEST; i++) {
				if ((*it)->getRequest(i) != NULL && !(*it)->getRequest(i)->checkTimeout())
					(*it)->eraseRequest(i);
			}
		}
	}
	std::cout << _EMERALD _BOLD "Goodbye!" _END << std::endl;
}
