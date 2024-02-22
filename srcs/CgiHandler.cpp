# include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request * theRequest) : _theRequest(theRequest)
{
	_fdGet[READ] = -1;
	_fdGet[WRITE] = -1;
	_fdPost[READ] = -1;
	_fdPost[WRITE] = -1;
	
	craftEnv();
}

CgiHandler::~CgiHandler() 
{
	if (this->_fdGet[READ] != -1)
		close(this->_fdGet[READ]);
	if (this->_fdGet[WRITE] != -1)
		close(this->_fdGet[WRITE]);
	if (this->_fdPost[READ] != -1)
		close(this->_fdPost[READ]);
	if (this->_fdPost[WRITE] != -1)
		close(this->_fdPost[WRITE]);
}

std::string CgiHandler::retrieveServerName(std::string request)
{
	size_t		begin;
	size_t		end;
	std::string	server_name;

	begin = request.find("Referer: http://");
	if (begin != std::string::npos){
		server_name = request.substr(begin + 14, std::string::npos);
		end = server_name.find(":");
		if (end != std::string::npos)
			server_name = server_name.substr(0, end);
		std::cout << "Servername: " << server_name << std::endl;
		return server_name;
	}
	return NULL;
}

std::string CgiHandler::retrieveScriptName(std::string resource)
{
	size_t		begin;
	size_t		end;
	std::string	script_name;

	std::cout << "Resource: " << resource << std::endl;
	if (_theRequest->getMethod() == "POST"){
		begin = resource.find('/');
		script_name = resource.substr(begin + 1, std::string::npos);
		std::cout << "Script Name: " << script_name << std::endl;
		return script_name;
	}
	begin = resource.rfind('/');
	end = resource.find('?');
	script_name = resource.substr(begin + 1, end - begin -1);
	std::cout << "Script Name: " << script_name << std::endl;
	return script_name;
}

void CgiHandler::craftEnv(void)
{
	_envMap["GATEWAY_INTERFACE"] = "Our_CGI/1.1";
	// _envMap["SERVER_NAME"] = retrieveServerName(_theRequest->getRequest());;
	_envMap["SERVER_SOFTWARE"] = "WebSlayyy/1.0";
	_envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	// uint16_t Port = _theRequest->getCurrentServer()->getListen();
	// _envMap["SERVER_PORT"] = std::to_string(Port);
	_envMap["REQUEST_METHOD"] = _theRequest->getMethod();
	_envMap["PATH_INFO"] = retrieveScriptName(_theRequest->getResource());

}


// void	execCGIScript( std::string ressource, int event_socket )
// {
// 	(void)ressource;
// 	(void)event_socket;
	// pid_t			child_pid;
	// std::ifstream	cgi_script(ressource.c_str(), std::ios::binary);
	// std::string		php_path = "/usr/bin/php";
	// std::string		data = "key=value";
	// char	*args[] = { (char *)php_path.c_str(),(char *)ressource.c_str(), (char *)data.c_str(), NULL };
	// int		status;
	// int		pfd[2];
	// char	buffer[4096];
	// int		readed = 0;

	// if ( !cgi_script.is_open() ) {
	// 	std::cerr << "Error : could not open the cgi script : " << ressource << std::endl;
	// }
	// if ( pipe(pfd) < 0 ) {
	// 	perror("pipe");
	// }
	// child_pid = fork();
	// // std::cout << "cgi_path : " << ressource << std::endl;
	// if ( child_pid == - 1)
	// 	perror("fork");
	// else if ( !child_pid ) {
	// 	// std::cout << "args = " << args[0] << std::endl;
	// 	dup2(pfd[1], STDOUT_FILENO);
	// 	execvp((char *)args[0], args);
	// 	perror("execv");
	// 	exit(0);
	// }
	// else {
	// 	if ( waitpid(child_pid, &status, 0) > 0 ) {
	// 		if ( !WIFEXITED(status) ) {
	// 			std::cout << "An error was encountered while executing the CGI script" << std::endl;
	// 		}
	// 		else {
	// 			if ( (readed = read(pfd[0], buffer, sizeof(buffer))) < 0 )
	// 				perror("read");
	// 			buffer[readed] = 0;
	// 			// std::cout << "data from the CGI script : " << buffer << std::endl;
	// 		}
	// 	}
	// 	CGIResponse(std::string(buffer), event_socket);
	// }
// }

// void	CGIResponse( std::string outstream, int event_socket )
// {
	// 	std::string	header;

	// 	std::cout << "CGI Response" << std::endl;
	// 	header = "HTTP/1.1 200 OK";
	// 	header += "Content-Type: text/html\r\n\r\n";
	// 	if (send(event_socket, header.c_str(), header.size(), 0) < 0) {
	// 			std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
	// 	}
	// 	if (send(event_socket, outstream.c_str(), outstream.size(), 0) < 0) {
	// 			std::cout << _RED _BOLD "Error: SEND BUFFER" _END << std::endl;
	// 	}
// }



/*
If request == cgi && cgi == NULL
	create CGI
if != NULL
	launch CGI // 


*/