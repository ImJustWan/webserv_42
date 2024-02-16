#pragma once

# include "Response.hpp"

class Post : public Response {

public:

	Post( void );
	~Post();

	void		executeMethod();


private:

	Post(const Post& src);
	Post& operator=(const Post& cpy);

	void		writeFile();
	void		buildHeader();
	void		sendResponse();

};

	/* OBSOLETE ? */
	// void		fillMap();
	// std::string	getKey(size_t & found);
	// std::string	getValue(size_t & found, std::string const & queryKey);
	// std::string	extractToken( char *searched, size_t & found );