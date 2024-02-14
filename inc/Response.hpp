#pragma once

# include "Request.hpp"

struct header {
	int				status_code; // (?)
	std::string		date;
	std::string		server;
	std::string		content_type;
	std::string		content_length;
	bool			connection; // keep-alive/closed
	// std::string location;
};

class Response : public Request
{

public:

	Response();
	~Response();
	Response ( const Response& cpy );
	Response& operator= ( const Response& cpy );

	void				setAttributes();

	void				sendResponse();
	void				craftResponse();
	void				buildHeader( std::ifstream & file, unsigned int const & status_code );
	virtual void		executeMethod();
	void				responseError( const unsigned int & status_code );
	bool				accessCheck();
	bool				requestLineCheck();

	/* Status code  */
	std::map<unsigned int, std::string>	_status_code;

protected:
	std::string		_header;
	std::string		_body; // response

	std::string		_status;
	std::map<uint16_t, std::string>		_errors;


};
