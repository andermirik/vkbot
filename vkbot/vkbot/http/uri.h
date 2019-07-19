#pragma once
#include <string>
namespace http{
	class Uri {
	public:
		Uri(std::string);
		Uri();
		std::string str;
		std::string scheme();
		std::string host();
		std::string path();
		std::string args();
	};
}