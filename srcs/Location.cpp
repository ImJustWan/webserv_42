#include "webserv.hpp"

void Location::configurationMap(void)
{
	this->_configMap["location"] = &Location::setDirectory;
	this->_configMap["root"] = &Location::setRoot;
	this->_configMap["methods"] = &Location::setMethods;
	this->_configMap["index"] = &Location::setIndex;
	this->_configMap["file_ext"] = &Location::setFileExt;
	this->_configMap["cgi_path"] = &Location::setCgiPath;
	this->_configMap["autoindex"] = &Location::setAutoindex;
	this->_configMap["alias"] = &Location::setAlias;
}


Location::Location(std::vector<std::string> &tokens) : _autoindex(true), _methods(0)
{
	configurationMap();
	for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); ++i) {
		std::map<std::string, void (Location::*)(std::vector<std::string>)>::iterator configIt = this->_configMap.find(*i);
		std::vector<std::string> directive;
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
}

Location::~Location() {}


/* ****************  GETTERS **************** */

std::string const & Location::getDirectory(void) const { return this->_directory; }
std::string const & Location::getRoot(void) const { return this->_root; }
int const & Location::getMethods(void) const { return this->_methods; }
std::string const & Location::getIndex(void) const { return this->_index; }
std::string const & Location::getFileExt(void) const { return this->_fileExt; }
std::string const & Location::getCgiPath(void) const { return this->_cgiPath; }
bool const & Location::getAutoindex(void) const { return this->_autoindex; }
std::string const & Location::getAlias(void) const { return this->_alias; }


/* ****************  SETTERS **************** */
void Location::setDirectory(std::vector<std::string> location) {
	if (location.size() != 2)
		throw InvalidConfig(INVALLOC "Location Directive");
	this->_directory = location[1];

}

void Location::setRoot(std::vector<std::string> root) {
	std::string	data;
	data = dataExtractor<std::string>(root[1]);
	std::ifstream test(data.c_str());
	if (!test)
		throw InvalidConfig(INVALLOC "Root Directive");
	test.close();
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
		if (j == 5)
			throw InvalidConfig(INVALCONF "Unknown method");
	}
}

void Location::setIndex(std::vector<std::string> index) {
	std::string	data;
	data = dataExtractor<std::string>(index[1]);
	if (data.rfind(".html\0") == std::string::npos)
		throw InvalidConfig(INVALLOC "Index Directive");
	this->_index = data;
}

void Location::setFileExt(std::vector<std::string> fileExt) {
	if (fileExt.size() != 2)
		throw InvalidConfig(INVALLOC "File Extension Directive");
	this->_fileExt = fileExt[1];
}

void Location::setCgiPath(std::vector<std::string> cgiPath) {
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

void Location::setAlias(std::vector<std::string> alias) {
	if (alias.size() != 2)
		throw InvalidConfig(INVALLOC "Alias Directive");
	this->_alias = alias[1];
}