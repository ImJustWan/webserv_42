# include "CGI.hpp"

bool	isCGI( std::string const & ressource )
{
	if ( "cgi-bin" == ressource.substr(0, 7) )
		return ( true );
	return ( false );
}

void	CGIResponse( std::string outstream, int event_socket )
{
	std::string	header;

	std::cout << "CGI Response" << std::endl;
	header = "HTTP/1.1 200 OK";
	header += "Content-Type: text/html\r\n\r\n";
	if (send(event_socket, header.c_str(), header.size(), 0) < 0) {
			std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
	}
	if (send(event_socket, outstream.c_str(), outstream.size(), 0) < 0) {
			std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
	}
}

void	execCGIScript( std::string ressource, int event_socket )
{
	pid_t			child_pid;
	std::ifstream	cgi_script(ressource.c_str(), std::ios::binary);
	std::string		php_path = "/usr/bin/php";
	std::string		data = "key=value";
	char	*args[] = { (char *)php_path.c_str(),(char *)ressource.c_str(), (char *)data.c_str(), NULL };
	int		status;
	int		pfd[2];
	char	buffer[4096];
	int		readed = 0;

	if ( !cgi_script.is_open() ) {
		std::cerr << "Error : could not open the cgi script : " << ressource << std::endl;
	}
	if ( pipe(pfd) < 0 ) {
		perror("pipe");
	}
	child_pid = fork();
	// std::cout << "cgi_path : " << ressource << std::endl;
	if ( child_pid == - 1)
		perror("fork");
	else if ( !child_pid ) {
		// std::cout << "args = " << args[0] << std::endl;
		dup2(pfd[1], STDOUT_FILENO);
		execvp((char *)args[0], args);
		perror("execv");
		exit(0);
	}
	else {
		if ( waitpid(child_pid, &status, 0) > 0 ) {
			if ( !WIFEXITED(status) ) {
				std::cout << "An error was encountered while executing the CGI script" << std::endl;
			}
			else {
				if ( (readed = read(pfd[0], buffer, sizeof(buffer))) < 0 )
					perror("read");
				buffer[readed] = 0;
				// std::cout << "data from the CGI script : " << buffer << std::endl;
			}
		}
		CGIResponse(std::string(buffer), event_socket);
	}
}
