#pragma once
#include "uri.h"
#include "request.h"
#include "responce.h"

namespace http {

	Response get(std::string const& path, std::string const& data = "");
	Response post(std::string const& path, std::string const& data = "");

	Response sendRequest(Request&request);
	Response sendHTTPS(Request& request);
	Response sendHTTP(Request& request);

	std::string str(Request& request);
	std::string str(Response& responce);

}