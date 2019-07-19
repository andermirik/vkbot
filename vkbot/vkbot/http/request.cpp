#include "request.h"
#include <sstream>

http::Request::Request()
{
}

std::string http::Request::to_string()
{
	std::stringstream ss;
	ss << method << " /" << uri.path() + "?" + uri.args() << " HTTP/1.1\n";
	ss << "Host: " << uri.host() << "\n";
	ss << "Content-Length: " << body.size() << "\n";
	ss << "Connection: close\n";
	ss << "Accept-Encoding: identity\n";
	for (auto header : headers) {
		ss << header.first << ": " << header.second << "\n";
	}
	ss << "\n";
	ss << body;
	return ss.str();
}
