#include "webserv.hpp"

void Location::configurationMap(void)
{
	this->_configMap["location"] = &Location::setDirectory;
	this->_configMap["methods"] = &Location::setMethods;
	this->_configMap["root"] = &Location::setRoot;
	this->_configMap["index"] = &Location::setIndex;
	this->_configMap["file_ext"] = &Location::setFileExt;
	this->_configMap["cgi_path"] = &Location::setCgiPath;
	this->_configMap["autoindex"] = &Location::setAutoindex;
	this->_configMap["upload_path"] = &Location::setUploadPath;
	this->_configMap["rewrite"] = &Location::setRewrite;
}

void Location::BlackList(void)
{
	this->_blackList.insert("listen");
	this->_blackList.insert("client_max_body_size");
	this->_blackList.insert("server_name");
	this->_blackList.insert("error_page");
}

Location::Location(std::vector<std::string> &tokens, Server * currentServer) : _autoindex(true), _methods(0), _currentServer(currentServer)
{
 	BlackList();
	configurationMap();
	for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i) {
		std::map<std::string, void (Location::*)(std::vector<std::string>)>::iterator configIt = this->_configMap.find(*i);
		std::set<std::string>::iterator isBlackListed = this->_blackList.find(*i);
		std::vector<std::string> directive;
		if (isBlackListed != this->_blackList.end())
		{
			std::cout << *isBlackListed << std::endl;
			throw InvalidConfig(INVALLOC "Black listed member in Location");
		}
		if (configIt != this->_configMap.end()) {
			while (i != tokens.end() && *i != ";" && *i != "{" && *i != "}") {
				directive.push_back(*i);
				++i;
			}
			if (directive.size() < 2)
				throw InvalidConfig(INVALLOC "Incomplete Directive");
			(this->*configIt->second)(directive);
		}
	}
	checkMinimumConf();
}

Location::~Location() {}

void Location::checkMinimumConf(void)
{
	if (this->_root.size() == 0) {
		this->_root = _currentServer->getRoot();
		std::string	dirpath = this->_root + this->_directory;
		std::ifstream testDir(dirpath.c_str());
		if (!testDir)
			throw InvalidConfig(INVALLOC "Location Directory Directive");
		testDir.close();
	}
	else if (this->_root != _currentServer->getRoot()) {
		std::ifstream testRoot(this->_root.c_str());
		if (!testRoot)
			throw InvalidConfig(INVALLOC "Location Root Directive: ");
		testRoot.close();
		this->_directory = this->_root + this->_directory;
		std::ifstream testDir(this->_directory.c_str());
		if (!testDir)
			throw InvalidConfig(INVALLOC "Location Directory Directive");
		testDir.close();
		this->_directory = '/' + this->_directory;
	}
	if (this->_rewrite.size() > 1)
		throw InvalidConfig(INVALLOC "Too many redirections");
}

/* ****************  GETTERS **************** */

std::string const & Location::getDirectory(void) const { return this->_directory; }
std::string const & Location::getRoot(void) const { return this->_root; }
int const & Location::getMethods(void) const { return this->_methods; }
std::string const & Location::getIndex(void) const { return this->_index; }
std::string const & Location::getFileExt(void) const { return this->_fileExt; }
std::string const & Location::getCgiPath(void) const { return this->_cgiPath; }
bool const & Location::getAutoindex(void) const { return this->_autoindex; }
std::string const & Location::getUploadPath(void) const { return this->_uploadPath; }
std::map<int, std::string> const & Location::getRewrite(void) const { return this->_rewrite; }


/* ****************  SETTERS **************** */

void Location::setDirectory(std::vector<std::string> location) {
	if (location.size() != 2)
		throw InvalidConfig(INVALLOC "Location Directive");
	this->_directory = location[1];

}

void Location::setRoot(std::vector<std::string> root)
{
	if (this->_root.empty() == false)
		throw InvalidConfig(INVALLOC "Duplicate Root Directive");
	std::string	data;
	data = dataExtractor<std::string>(root[1]);
	this->_root = data;
}

void Location::setMethods(std::vector<std::string> methods) {
	
	std::string methodsID[5] = {"", "GET", "POST", "", "DELETE"};
	for(std::vector<std::string>::iterator i = methods.begin() + 1; i!= methods.end(); ++i)
	{
		size_t j = 1;
		for(; j < 5 ; ++j)
			if (*i == methodsID[j]){
				this->_methods |= j;
				break ;
			}
		if (j == 5){
			
			throw InvalidConfig(INVALCONF "Unknown method");
		}
	}
}

void Location::setMethods(int methods)
{
	if (this->_methods != 0)
		throw InvalidConfig(INVALCONF "Duplicate method");
	if (methods < 0 || methods > 7)
		throw InvalidConfig(INVALCONF "Unknown method");
	this->_methods = methods;
}

void Location::setIndex(std::vector<std::string> index)
{
	if (this->_index.empty() == false)
		throw InvalidConfig(INVALLOC "Duplicate Index Directive");
	std::string	data;
	data = dataExtractor<std::string>(index[1]);
	if (data.rfind(".html\0") == std::string::npos)
		throw InvalidConfig(INVALLOC "Index Directive");
	this->_index = data;
}

void Location::setFileExt(std::vector<std::string> fileExt)
{
	if (this->_fileExt.empty() == false)
		throw InvalidConfig(INVALLOC "Duplicate FileExt Directive");
	if (fileExt.size() != 2)
		throw InvalidConfig(INVALLOC "File Extension Directive");
	this->_fileExt = fileExt[1];
}

void Location::setCgiPath(std::vector<std::string> cgiPath)
{
	if (this->_cgiPath.empty() == false)
		throw InvalidConfig(INVALLOC "Duplicate CGI path Directive");
	if (cgiPath.size() != 2)
		throw InvalidConfig(INVALLOC "CGI Directive");
	this->_cgiPath = cgiPath[1];
}

void Location::setAutoindex(std::vector<std::string> autoindex) {
	if (autoindex[1] == "on")
		this->_autoindex = true;
	else if (autoindex[1] == "off")
		this->_autoindex = false;
	else
		throw InvalidConfig(INVALLOC "Autoindex Directive");
}

void Location::setUploadPath(std::vector<std::string> UploadPath)
{
	if (this->_uploadPath.empty() == false)
		throw InvalidConfig(INVALLOC "Duplicate upload path Directive");
	if (UploadPath.size() != 2)
		throw InvalidConfig(INVALLOC "Upload Path Directive");
	std::string	data;
	data = dataExtractor<std::string>(UploadPath[1]);
	this->_uploadPath = data;
}

void Location::setRewrite(std::vector<std::string> rewrite)
{
	if (this->_rewrite.empty() == false)
		throw InvalidConfig(INVALLOC "Duplicate rewrite Directive");
	if (rewrite.size() != 3)
		throw InvalidConfig(INVALLOC "Rewrite Directive");
	int error_code;
	error_code = dataExtractor<int>(rewrite[1]);
	if (error_code != 301 && error_code != 302)
		throw InvalidConfig(INVALLOC "Rewrite Directive - Invalid error code");
	std::string		path;
	path = dataExtractor<std::string>(rewrite[2]);
	this->_rewrite[error_code] = path;
}
