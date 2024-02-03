# include "ServerHandler.hpp"

ServerHandler::ServerHandler() {
}

ServerHandler::~ServerHandler() {
}

// void	ServerHandler::closeServers( Server& servers )
// {

// }


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

void	ServerHandler::processHandler()
{
	addMasterSockets();

	struct epoll_event readyList[30];

	int readyz;
	while (1)
	{
		std::cerr << "WOOP WOOP" << std::endl;
		readyz = epoll_wait(this->_epfd, readyList, 30, 5000);
		if (readyz != 0)
		{
			for (int i = 0; i < readyz; i++) {
				reinterpret_cast<IEvent *>(readyList[i].data.ptr)->determinism();
			}
		}
		else
			std::cout << "No events :(" << std::endl;
	}
}

void	ServerHandler::createServers()
{
	Server	*serv = new Server;

	this->_servers.push_back(serv);
}

void	ServerHandler::handleServers()
{
	createServers();
	this->_epfd = epoll_create(1);
	processHandler();
}
