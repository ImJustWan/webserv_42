# include "Server.hpp"
# include "colors.hpp"
# include "ServerHandler.hpp"

volatile sig_atomic_t signal_received = false;

/*
	TO DO

		- for GET and responseError : 
			check EVERY read/recv/send return for 0 AND -1
		- check again all read/recv/write/send

		- check all error status code

		- from eval grid
			* Limit the client body (use: curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something
			shorter or longer than body limit").
			* The CGI should be run in the correct directory for relative path file access.
			* use a script containing an infinite loop or an error (timeout closes socket ?)
			* In the configuration, try to setup the same port multiple times. It should not work. ???
				// if no different server_name ?

		- www concatenation of root + current page
		
		- from syscall_fix get http protocol_check
			
	if (_contentLength > _currentServer->getClientMaxBodySize())
		-  error 
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
