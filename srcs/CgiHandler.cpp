# include "CgiHandler.hpp"

CgiHandler::CgiHandler(Request * theRequest) : _theRequest(theRequest)
{
	std::cout << _BOLD _YELLOW "CREATE CGI" _END << " Method->" << this->_theRequest->getMethod() << std::endl;
	this->_CgiStatus = 1;
	try {
		this->_time = time(NULL);
		this->_argv = NULL;
		this->_envp = NULL;
		_fds[READ] = -1;
		_fds[WRITE] = -1;
		_fdPost[READ] = -1;
		_fdPost[WRITE] = -1;
		this->_script_name = retrieveScriptName(_theRequest->getResource());
		craftEnv();
		setFds();
		setArgv();
		this->_CgiStatus = 2;
	}
	catch (ErrorInCGI &e){
		std::cout << _RED _BOLD << "Error: " << _END << e.what() << std::endl;
		this->_CgiStatus = 5;
		this->_theRequest->buildResponse(e.getError());
	}
}

CgiHandler::~CgiHandler() 
{
	if (this->_fds[READ] != -1)
		::close(this->_fds[READ]);
	if (this->_fds[WRITE] != -1)
		::close(this->_fds[WRITE]);
	if (this->_fdPost[READ] != -1)
		::close(this->_fdPost[READ]);
	if (this->_fdPost[WRITE] != -1)
		::close(this->_fdPost[WRITE]);
	if (this->_envp != NULL){
		for (int i = 0; _envp[i]; ++i)
			delete [] _envp[i];
		delete [] this->_envp;
	}
	if (this->_argv != NULL){
		for (int i = 0; _argv[i]; ++i)
			delete [] _argv[i];
		delete [] this->_argv;
	}
}

uint8_t	const &	CgiHandler::getCgiStatus(void) const {
	return this->_CgiStatus;
}

void	CgiHandler::setCgiStatus(uint8_t status) {
	if (status < 1 || status > 5)
		throw ErrorInCGI("Invalid CGI status code", 500);
}

std::string	CgiHandler::retrieveServerName(std::string request)
{
	size_t		begin;
	size_t		end;
	std::string	server_name;

	begin = request.find("Host: ");
	if (begin != std::string::npos){
		server_name = request.substr(begin + 6, std::string::npos);
		end = server_name.find(":");
		if (end != std::string::npos)
			server_name = server_name.substr(0, end);
		return server_name;
	}
	return NULL;
}

std::string	CgiHandler::retrieveScriptName(std::string resource)
{
	size_t		begin;
	size_t		end;
	std::string	script_name;

	if (_theRequest->getMethod() == "POST"){
		begin = resource.find('/');
		script_name = resource.substr(begin + 1, std::string::npos);
		return script_name;
	}
	begin = resource.rfind('/');
	end = resource.find('?');
	script_name = resource.substr(begin + 1, end - begin -1);
	return script_name;
}

void	CgiHandler::retrievePathInfo(std::string resource)
{
	size_t		end;

	if (_theRequest->getMethod() == "POST"){
		this->_path_info = "." + resource;
	}
	else {
		end = resource.find('?');
		this->_path_info = "." + resource.substr(0, end);
	}
}

void	CgiHandler::retrieveQueryString(std::string resource)
{
	size_t		begin;

	if (_theRequest->getMethod() == "POST"){
		this->_query_string = "";
	}
	else {
		begin = resource.find('?');
		this->_query_string = resource.substr(begin + 1, std::string::npos);
	}
}

std::string	CgiHandler::envContentLen(void)
{
	if (this->_theRequest->getMethod() == "GET")
		return "0";
	size_t bodyDelim = this->_theRequest->getRequest().find("\r\n\r\n");
	if (bodyDelim != std::string::npos)
		this->_requestBody = this->_theRequest->getRequest().substr(bodyDelim + 4, this->_theRequest->getRequest().size() - bodyDelim);
	std::ostringstream content_len;
	content_len << this->_requestBody.size();
	return content_len.str();
}

std::string	CgiHandler::envContentType(void)
{
	size_t begin = this->_theRequest->getRequest().find("Content-Type: ");
	size_t end;
	std::string contentType = "";
	if (begin != std::string::npos){
		begin += 14;
		contentType = this->_theRequest->getRequest().substr(begin, std::string::npos);
		end = contentType.find('\n');
		contentType = contentType.substr(0, end);
	}
	return contentType;
}


void	CgiHandler::craftEnv(void)
{
	retrievePathInfo(_theRequest->getResource());
	retrieveQueryString(_theRequest->getResource());
	std::ostringstream port;
	port << _theRequest->getListen();

	_envMap["DOCUMENT_ROOT"] = this->_path_info;
	if ( _theRequest->getMethod() == "POST"){
		_envMap["CONTENT_LENGTH"] = envContentLen();
		_envMap["CONTENT_TYPE"] = envContentType();
	}
	_envMap["GATEWAY_INTERFACE"] = "Our_CGI/1.1";
	_envMap["PATH_INFO"] = this->_path_info;
	_envMap["PATH_TRANSLATED"] = this->_path_info;
	_envMap["QUERY_STRING"] = this->_query_string;
	_envMap["REDIRECT_STATUS"] = "200";
	_envMap["REQUEST_METHOD"] = _theRequest->getMethod();
	_envMap["SCRIPT_NAME"] = this->_script_name;
	_envMap["SERVER_NAME"] = retrieveServerName(_theRequest->getRequest());
	_envMap["SERVER_PORT"] = port.str();
	_envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	_envMap["SERVER_SOFTWARE"] = "WebSlayyy/1.0";

	char **envp = new char*[_envMap.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _envMap.begin(); it != _envMap.end(); ++it)
	{
		// std::cout << _LILAC << it->first << " = " << it->second << _END << std::endl;
		std::string envExport = it->first + "=" + it->second;
		envp[i] = new char[envExport.size() + 1];
		strcpy(envp[i], envExport.c_str());
		i++;
	}
	envp[i] = NULL;
	_envp = envp;
}

void CgiHandler::setFds(void)
{
	if (_theRequest->getMethod() == "POST"){
		if (::pipe(_fdPost) == -1)
			throw ErrorInCGI("Post Pipe Failed", 500);
		fcntl(_fdPost[1], F_SETFL, fcntl(_fdPost[1], F_GETFL) | O_NONBLOCK);
	}
	else {
		_fdPost[READ] = -1;
		_fdPost[WRITE] = -1;
	}
	if (::pipe(_fds) == -1)
		throw ErrorInCGI("Child Pipe Failed", 500);
	fcntl(_fds[0], F_SETFL, fcntl(_fds[0], F_GETFL) | O_NONBLOCK);
}

void	CgiHandler::setArgv(void)
{
	accessChecks(this->_path_info, this->_theRequest->getLocation()->getCgiPath());
 	
	char **argv = new char*[3];
	argv[0] = new char[this->_theRequest->getLocation()->getCgiPath().size() + 1];
	argv[1] = new char[this->_path_info.size() + 1];
	::strcpy(argv[0], this->_theRequest->getLocation()->getCgiPath().c_str());
	::strcpy(argv[1], this->_path_info.c_str());
	argv[2] = NULL;

	this->_argv = argv;
}

void	CgiHandler::accessChecks(std::string script, std::string interpreter)
{
	if (::access(script.c_str(), F_OK) != 0)
		throw ErrorInCGI("No access to script", 403);
	if (::access(script.c_str(), R_OK) != 0)
		throw ErrorInCGI("Access to script forbidden", 403);
	if (::access(interpreter.c_str(), F_OK) != 0)
		throw ErrorInCGI("No Access to interpreter", 403);
	if (::access(interpreter.c_str(), X_OK) != 0)
		throw ErrorInCGI("Access to interpreter forbidden", 403);
}

void	CgiHandler::execCGI(void)
{
	try {
		std::cout << _BOLD _YELLOW "EXEC CGI" _END << " Method->" << this->_theRequest->getMethod() << std::endl;
		if (this->_CgiStatus == 3) {
			execParent();
			return;
		}
		if (this->_CgiStatus == 2) {
			this->_pid = ::fork();
			if (this->_pid == -1)
				throw ErrorInCGI("Child Fork failed", 500);
			if (this->_pid == 0)
				execChild();
			this->_CgiStatus = 3;
			return ;
		}
	}
	catch (ErrorInCGI &e){
		std::cout << _RED _BOLD << "Error: " << _END << e.what() << std::endl;
		this->_CgiStatus = 5;
		this->_theRequest->buildResponse(e.getError());
		return;
	} 
}

void	CgiHandler::execParent(void)
{
	int		status;
	pid_t	finisher;
	if (time(NULL) - this->_time < 2) {
		finisher = waitpid(this->_pid, &status, WNOHANG);
		if (finisher == this->_pid){
			char *buffer = new char[READ_BUFFER_SIZE];
			int size = ::read(_fds[READ], buffer, READ_BUFFER_SIZE);
			if (size < 0)
			{
				delete [] buffer;
				if (::close(_fds[READ]) < 0|| ::close(_fds[WRITE]) < 0)
					throw ErrorInCGI("Close failed for _fds", 500);
				throw ErrorInCGI("Reading buffer", 500);
			}
			if (size == 0)
			{
				if (::close(_fds[READ]) < 0|| ::close(_fds[WRITE]) < 0)
					throw ErrorInCGI("Close failed for _fds", 500);
			}
			else
			{
				buffer[size] = '\0';
				this->_response = buffer;
			}
			this->_CgiStatus = 4;
			delete [] buffer;
		}
	}
	else {
		kill(this->_pid, SIGKILL);
		throw ErrorInCGI("Child TimedOut", 504);
	}
}

void	CgiHandler::execChild(void)
{
	if (::dup2(this->_fds[WRITE], STDOUT_FILENO) < 0)
		throw ErrorInCGI("Dup2 failed for _fds[WRITE]", 500);
	if (::close(this->_fds[WRITE]) < 0 || ::close(this->_fds[READ]) < 0)
		throw ErrorInCGI("Close failed for _fds", 500);
	this->_fds[READ] = -1;
	if (this->_theRequest->getMethod() == "POST") {
		if (::dup2(this->_fdPost[READ], STDIN_FILENO) < 0)
			throw ErrorInCGI("Dup2 failed for _fdPost[READ]", 500);
		if (::write(this->_fdPost[WRITE], this->_requestBody.c_str(), this->_requestBody.size()) <= 0)
			throw ErrorInCGI("write to post failled", 500);
		if (::close(this->_fdPost[WRITE]) < 0)
			throw ErrorInCGI("Close failed for _fdPost[WRITE]", 500);
		this->_fdPost[WRITE] = -1;
	}
	::execve(this->_argv[0], this->_argv, this->_envp);
	throw ErrorInCGI("Execve Failed", 500);
}

void	CgiHandler::createCgiHeader(void)
{
	std::string response = _response;
	std::ostringstream file_size_str;
	file_size_str << _response.size();
	std::string tmp = file_size_str.str();

	_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
	_response += "Content-Length: " + tmp + "\r\n\r\n";
	_response += response;
}

void	CgiHandler::checkCgiHeader(void)
{
	size_t emptyLinePos = _response.find("\r\n\r\n");

	if (emptyLinePos == std::string::npos) {
		emptyLinePos = _response.find("\n\n");
		if (emptyLinePos == std::string::npos) {
			createCgiHeader();
			return ;
		}
	}

	std::string header = _response.substr(0, emptyLinePos);
	if (header.find("HTTP/1.1") != std::string::npos
		&& header.find("Content-Type:") != std::string::npos
		&& header.find("Content-Length:") != std::string::npos)
		return;
	else
		createCgiHeader();
}

void	CgiHandler::sendResponse(void)
{
	std::cout << _BOLD _YELLOW "CGI RESPONSE" _END  << " Method->" << this->_theRequest->getMethod() << std::endl;
	checkCgiHeader();
	this->_theRequest->setFinalResponse(this->_response);
}