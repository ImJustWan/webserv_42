# include "webserv.hpp"

std::string	Timestamp( void ) {

	struct tm *timeinfo;
	std::time_t result;

	std::time(&result);
	timeinfo = std::localtime(&result);
	std::stringstream timestamp;
	timestamp << 1900 + timeinfo->tm_year << 1 + timeinfo->tm_mon << timeinfo->tm_mday << "_" << timeinfo->tm_hour << timeinfo->tm_min;
	return timestamp.str();
}

std::string	TimestampDetailed( void ) {

	struct tm *timeinfo;
	std::time_t result;

	std::time(&result);
	timeinfo = std::localtime(&result);
	std::stringstream timestamp;
	timestamp << 1900 + timeinfo->tm_year << "-" << 1 + timeinfo->tm_mon << "-" << timeinfo->tm_mday;
	timestamp << "_" << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec;
	return timestamp.str();
}

void	logRequest(std::string request, std::string logpath, int error_code)
{
	std::ofstream	logfile;

	if (request.empty())
		return;
	logfile.open(logpath.c_str(), std::ios_base::app);

	logfile << TimestampDetailed() << "\t[";

	size_t	reqEnd = request.find("\n");
	if (reqEnd != std::string::npos)
			logfile << "\"" << request.substr(0, reqEnd - 1) << "\"" << " - ";

	if (error_code != 0)
	{
		std::stringstream status_code_str;
		status_code_str << error_code;
		logfile << "\"Response Error : " << status_code_str.str() << "\"";
	}
	else
	{
		size_t	hostBegin = request.find("Host: ");
		if (hostBegin != std::string::npos){
			size_t hostEnd = request.find("\n", hostBegin  + 7);
			if (hostEnd != std::string::npos)
				logfile << "\"" << request.substr(hostBegin + 6, (hostEnd - hostBegin - 7) ) << "\"" << " - ";
		}

		size_t	usAgBegin = request.find("User-Agent: ");
		if (usAgBegin != std::string::npos){
			size_t usAgEnd = request.find("\n", usAgBegin  + 13);
			if (usAgEnd != std::string::npos)
				logfile << "\"" << request.substr(usAgBegin + 12, (usAgEnd - usAgBegin - 13) ) << "\"";
		}
	}
	logfile << "]" << std::endl;
	logfile.close();
}
