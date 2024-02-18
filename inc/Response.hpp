#pragma once

# include "Request.hpp"


class Response : public Request {

public:

	Response();
	~Response();
	
	Location *				getLocation(void) const;
	Request *				getCurrentRequest(void) const;

	void			setCurrentRequest( Request *current );

	void			setAttributes();
	void			responseError( const unsigned int & status_code );
	bool			requestLineCheck();
	void			trimSlash();

	virtual void	executeMethod();


protected:

	Response(const Response& cpy);
	Response& operator=(const Response& cpy);

	void			buildHeader( std::ifstream & file, unsigned int const & status_code );
	void			sendResponse();

	std::string		_header;
	std::string		_body;
	std::string		_status;


private :


	bool			extensionCheck();
	Request *		_currentRequest;
	std::map<unsigned int, std::string>	_status_code;

};

// OBSOLETE ?
// struct header {
// 	int				status_code; // (?)
// 	std::string		date;
// 	std::string		server;
// 	std::string		content_type;
// 	std::string		content_length;
// 	bool			connection; // keep-alive/closed
// 	// std::string location;
// };