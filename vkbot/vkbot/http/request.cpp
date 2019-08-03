#include "request.h"
#include <sstream>

http::Request::Request()
{
}

std::string http::Request::to_string()
{
	std::stringstream ss;
	ss << method<< " " << uri.path + (uri.query.empty()? "" : "?" + uri.query) << " HTTP/1.1\r\n";
	
	if (headers.find("Host") == headers.end())
		headers["Host"] = uri.host;

	if (headers.find("Content-Length") == headers.end())
		headers["Content-Length"] = std::to_string(body.size());

	if (headers.find("Connection") == headers.end())
		headers["Connection"] = "close";

	if (headers.find("Accept-Encoding") == headers.end())
		headers["Accept-Encoding"] = "identity";

	for (auto header : headers) {
		ss << header.first << ": " << header.second << "\r\n";
	}
	ss << "\r\n";
	ss << body;
	return ss.str();
}
