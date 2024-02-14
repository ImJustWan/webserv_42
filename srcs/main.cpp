# include "Server.hpp"
# include "colors.hpp"
# include "ServerHandler.hpp"

volatile sig_atomic_t signal_received = false;

int main(int ac, char **av)
{
	try {
		if (ac == 1)
		{
			ServerHandler	launcher;
			launcher.handleServers();
		}
		else if (ac == 2)
		{
			ServerHandler	launcher(static_cast<std::string>(av[1]));
			launcher.handleServers();
		}
		else
		{
			std::cout << _MAGENTA _BOLD "Invalid Arguments! " _END _MAGENTA "Usage: ./Webserv [configuration file]" _END << std::endl;
			return 1;
		}
	}
	catch (std::exception &e){
		std::cout << "Error: " << e.what() << std::endl;
	}
	return 0;
}
