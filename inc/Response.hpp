#pragma once

# include "Request.hpp"
# include "error.hpp"


class Response : public Request {

public:

	Response();
	~Response();

	void			setAttributes();
	void			responseError( const uint16_t & status_code );
	void			errorPageBuilder(const uint16_t & status_code);
	bool			requestLineCheck();

	virtual void	executeMethod();


protected:

	Response(const Response& cpy);
	Response& operator=(const Response& cpy);

	void			buildHeader( std::ifstream & file, uint16_t const & status_code );
	void			sendResponse();


	std::string		_header;
	std::string		_body;
	std::string		_status;


private :


	bool			extensionCheck();

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