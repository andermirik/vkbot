#pragma once
#include <string>

namespace http{
	class Uri {
	public:
		std::string str;

		std::string scheme;
		std::string host;
		std::string path;
		std::string query;
		std::string fragment;

		std::string file_format;

		Uri(std::string);
		Uri();

		void from_string(std::string str);
	};
}