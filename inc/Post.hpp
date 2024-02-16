#pragma once

# include "Response.hpp"
# define MAX_FILE_SIZE 10 * 1024 * 1024 // 1MB


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
	std::string extractBoundary();


};

	/* OBSOLETE ? */
	// void		fillMap();
	// std::string	getKey(size_t & found);
	// std::string	getValue(size_t & found, std::string const & queryKey);
	// std::string	extractToken( char *searched, size_t & found );