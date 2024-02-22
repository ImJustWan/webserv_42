#pragma once

# include "Response.hpp"

class Get : public Response {

public:

	Get( void );
	~Get();

	void executeMethod();


private:

	Get(const Get& src);
	Get& operator=(const Get& cpy);

	void				getAutoIndex();
	void				sendResponse();
	bool				checkResource();
	unsigned long int	getFileSize();

	std::ifstream	_file;

};
