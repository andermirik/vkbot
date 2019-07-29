#pragma once
#include <map>
#include <vector>
#include <sstream>

namespace http {
	class Response {
	private:
		int status_code;
		std::string body;
	public:
		std::map<std::string, std::vector<std::string>> headers;

		Response();
		Response(std::string str);
		void fromString(std::string str);
		std::string readUntilChr(std::string& src, char until, int& pos);
		std::string to_string();
		std::istream& safeGetline(std::istream& is, std::string& t);
		
		int Status_code();
		std::string Body();
	};
}