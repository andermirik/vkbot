#pragma once
#include <map>
#include <vector>
#include <sstream>

namespace http {
	class Response {
	public:
		int status_code;
		std::string body;
	public:
		std::map<std::string, std::string> headers;

		Response();
		Response(std::string& str);
		void fromString(std::string& str);
		std::string to_string();
		std::istream& safeGetline(std::istream& is, std::string& t);
	};
}