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

	std::string	_filename;
	std::string	_uploadedPath;
	std::string	_boundary;
	std::string	_data;

	void		startUpload();
	bool		uploadFile();
	void		buildHeader();
	void		sendResponse();
	void		extractBoundary();

};
