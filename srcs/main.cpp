# include "../inc/webserv.hpp"

int main () {

	try {
		ServerHandler	handler;
		handler.handleServers();
	}
	catch (std::exception &e)
	{
		perror(e.what());
		return 1;
	}
	
	return 0;
}