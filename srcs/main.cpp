# include "Server.hpp"
# include "colors.hpp"
# include "ServerHandler.hpp"

volatile sig_atomic_t signal_received = false;

/*
	TO DO

		- check all error status code

		- from eval grid
			* The CGI should be run in the correct directory for relative path file access.
			* use a script containing an infinite loop or an error (timeout closes socket ?)
			* In the configuration, try to setup the same port multiple times. It should not work. ???
				// if no different server_name ?
			
*/

int main(int ac, char **av)
{
	try {
		if (ac == 1 || ac == 2)
		{
			ServerHandler launcher(ac == 2 ? static_cast<std::string>(av[1]) : DEFAULT_CONF);
			launcher.handleServers();
		}
		else
		{
			std::cout << _MAGENTA _BOLD "Invalid Arguments! " _END _MAGENTA "Usage: ./Webserv [configuration file]" _END << std::endl;
			return 1;
		}
	}
	catch (std::exception &e){
		std::cout << _RED _BOLD "Error: " _END << e.what() << std::endl;
	}
	return 0;
}
