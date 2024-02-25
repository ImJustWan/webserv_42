#pragma once

# include "Response.hpp"

#define READ_BUFFER_SIZE 4096 * 32

class Get : public Response {

public:

	Get( void );
	~Get();

	void	executeMethod();
	void	getAutoIndex();


private:

	Get(const Get& src);
	Get& operator=(const Get& cpy);

	void				sendResponse();
	void				handleFile();
	// bool				checkResource();
	void				readChunk();
	// void				buildHeader();
	unsigned long int	getFileSize();

	std::ifstream		_file;
	std::vector<char>	_buffer;

};
