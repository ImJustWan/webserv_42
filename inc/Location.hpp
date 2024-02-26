/****** LOCATION MINIMUM CONFIGURATION ******/
/*
	Location should specify a directory;

	The rest is TBD depending on the request/response process
*/

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "webserv.hpp"

# define INVALLOC _BOLD _LILAC "Invalid Location: " _END

class Server;

class Location {

public:
	Location(std::vector<std::string> &tokens, Server *);
	~Location();

	/* GETTERS */
	std::string const & getDirectory(void) const;
	std::string const & getRoot(void) const;
	int const & getMethods(void) const;
	std::string const & getIndex(void) const;
	std::string const & getFileExt(void) const;
	std::string const & getCgiPath(void) const;
	bool const & getAutoindex(void) const;
	std::string const & getUploadPath(void) const;
	std::map<int, std::string> const & getRewrite(void) const;

	/* SETTERS */
	void setDirectory(std::vector<std::string>);
	void setRoot(std::vector<std::string>);
	void setMethods(std::vector<std::string>);
	void setMethods(int methods);
	void setIndex(std::vector<std::string>);
	void setFileExt(std::vector<std::string>);
	void setCgiPath(std::vector<std::string>);
	void setAutoindex(std::vector<std::string>);
	void setUploadPath(std::vector<std::string>);
	void setRewrite(std::vector<std::string>);

	class InvalidConfig : public std::exception {
	public:
		InvalidConfig(const char * msg) : _message(msg) {}
		virtual ~InvalidConfig() throw() {};
		virtual const char* what() const throw() {return this->_message;}
	private:
		const char * _message;
	};

private:

	void BlackList(void);
	void configurationMap(void);

	Location();

	Location & operator=(Location const &src);
	Location(Location const &src);

	void checkMinimumConf(void);

	template <typename T>
	T dataExtractor(std::string const &src)
	{
		std::stringstream	ss(src);
		T value;
		ss >> value;
		return value;
	}

	std::string		_directory;
	std::string		_root;
	std::string		_uploadPath;
	std::string		_index;
	bool			_autoindex;
	int				_methods;
	std::string		_fileExt;
	std::string		_cgiPath;
	Server *		_currentServer;
	std::map<int, std::string>	_rewrite;

	std::map<std::string, void(Location::*)(const std::vector<std::string>)> _configMap;
	std::set<std::string> _blackList;

};

# include "Server.hpp"

inline std::ostream & operator<<(std::ostream & o, Location const & rhs)
{
	std::map<int, std::string> rewrite = rhs.getRewrite();

	o << _BOLD " · Locations\t\t\t" _END << rhs.getDirectory() << "\n";
	o << _BOLD "    ◦ Root\t\t\t" _END << rhs.getRoot() << "\n";
	o << _BOLD "    ◦ Index\t\t\t" _END << rhs.getIndex() << "\n";
	o << _BOLD "    ◦ Autoindex\t\t\t" _END << (rhs.getAutoindex()?"true":"false") << "\n";
	o << _BOLD "    ◦ Methods\t\t\t" _END << rhs.getMethods() << "\n";
	o << _BOLD "    ◦ File extension\t\t" _END << rhs.getFileExt() << "\n";
	o << _BOLD "    ◦ CGI path\t\t\t" _END << rhs.getCgiPath() << std::endl;
	o << _BOLD "    ◦ Upload Path\t\t\t" _END << rhs.getUploadPath() << "\n";
	for (std::map<int, std::string>::iterator i = rewrite.begin(); i != rewrite.end(); ++i) 
		o << _BOLD "    ◦ Rewrite\t\t\t" _END << i->first << " " << i->second << "\n";

	return o;
};

#endif