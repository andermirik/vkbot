#pragma once
#include <map>
#include "uri.h"
namespace http {
	class Request {
	public:
		Request();
		std::string method;
		Uri uri;
		std::map<std::string, std::string> headers;
		std::string body;
		std::string to_string();
	};
}