#pragma once

# include "Response.hpp"

class Delete : public Response {

public:

	Delete( void );
	~Delete();
	void	executeMethod();
	void	sendResponse();

private:
	Delete ( const Delete& cpy );
	Delete& operator= ( const Delete& cpy );
	void	buildHeader();

};
