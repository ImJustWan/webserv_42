#pragma once

# include "Response.hpp"

class Post : public Response {
public:

	Post( void );
	~Post();

	void		executeMethod();
	void		fillMap();
	std::string	getKey(size_t & found);
	std::string	getValue(size_t & found, std::string const & queryKey);
	std::string	extractToken( char *searched, size_t & found );
	void		buildHeader();
	void		sendResponse();
	void		writeFile();

private:
	Post ( const Post& cpy );
	Post& operator= ( const Post& cpy );
	std::map<std::string, std::string>	_post_map;
};
