#include <string>
#include <sstream>
#include "uri.h"
#include <regex>

http::Uri::Uri(std::string str)
{
	this->str = str;
	from_string(str);
}

http::Uri::Uri()
{

}

void http::Uri::from_string(std::string str)
{
	std::match_results<const char*> res;
	std::regex uri("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");
	std::regex_search(str.c_str(), res, uri);

	scheme = res[2];
	host = res[4];
	path = res[5];
	query = res[7];
	fragment = res[9];

	std::regex file("\\.[a-z]*$");
	std::regex_search(path.c_str(), res, file);
	file_format = res[0];
	if (!file_format.empty())
		file_format = file_format.substr(1);
}
