#pragma once

# include "Response.hpp"

class Delete : public Response {

public:

	Delete( void );
	~Delete();

	void	executeMethod();


private:

	Delete(const Delete& src);
	Delete& operator=(const Delete& cpy);

	void	buildHeader();
	void	sendResponse();
	void	deleteResource();

};
