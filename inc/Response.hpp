#pragma once

# include "Request.hpp"
# include "error.hpp"


class Response {

public:

	Response();
	Response(Request const & src);
	virtual ~Response();
	
	std::string				getResponse(void) const;

	Request *				getCurrentRequest(void) const;
	void					setCurrentRequest( Request *current );

	void					responseError( const uint16_t & status_code );
	void					errorPageBuilder(const uint16_t & status_code);
	bool					requestLineCheck();
	bool					checkResource();
	std::string				trimSlash();

	virtual void			executeMethod();
	virtual void			getAutoIndex();


protected:

	Response(const Response& cpy);
	Response& operator=(const Response& cpy);

	void			buildHeader( std::ifstream & file, unsigned int const & status_code );
	void			sendResponse();

	std::string		_header;
	std::string		_body;
	std::string		_status;
	std::string		_response;


private :


	bool			extensionCheck();
	Request *		_currentRequest;
	std::map<unsigned int, std::string>	_status_code;

};
