#include <string>
#include <sstream>
#include "uri.h"

http::Uri::Uri(std::string str)
{
	this->str = str;
}

http::Uri::Uri()
{
}

std::string http::Uri::scheme() {
	std::stringstream ss;
	for (auto chr : str) {
		if (chr == ':')
			break;
		ss << chr;
	}
	return ss.str();
}

std::string http::Uri::host() {
	std::stringstream ss;
	int i = str.find("//") + 2;
	while (i < str.size()) {
		if (str[i] == '/')
			break;
		ss << str[i++];
	}
	return ss.str();
}
std::string http::Uri::path() {
	std::stringstream ss;
	int i = scheme().size() + 4 + host().size();
	if (!i)
		return "";
	while (i < str.size()) {
		if (str[i] == '?')
			break;
		ss << str[i++];
	}
	return ss.str();
}

std::string http::Uri::args() {
	std::stringstream ss;
	int i = str.find('?') + 1;
	if (!i)
		return "";
	while (i < str.size()) {
		if (str[i] == '#')
			break;
		ss << str[i++];
	}
	return ss.str();
}