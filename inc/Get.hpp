#pragma once

# include "Response.hpp"

class Get : public Response {

public:

	Get( void );
	~Get();
	void executeMethod();

private:
	Get ( const Get& cpy );
	Get& operator= ( const Get& cpy );
	void	sendResponse();


};
