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
	std::string	_fullPath;
	std::string	_boundary;
	std::string	_rawFileData;

	void		startUpload();
	void		uploadFile();
	void		buildHeader();
	void		buildResponse();
	void		extractBoundary();

	void		uploadChunkedFile();
	void		uploadBoundedFile();

	void		createFilename();
	void		createUploadFile();
	void		createUploadPath();

	class PostError : public std::exception {
	public:
		PostError(int code) : _errorCode(code) {}
		virtual ~PostError() throw() {};
		virtual int getErrorCode(void) const throw() {return this->_errorCode;}
		// virtual const char* what() const throw() {return "Post Error";}
	private:
		int _errorCode;
	};

};
