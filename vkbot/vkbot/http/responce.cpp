#include "responce.h"
#include <sstream>

http::Response::Response() {
	body = "";
}

http::Response::Response(std::string& str) {
	fromString(str);
}

void http::Response::fromString(std::string& src) {
	std::string line;
	std::stringstream str(src);

	str >> line;
	str >> status_code;
	safeGetline(str, line);

	for (line; safeGetline(str, line) && line!="";) {
		std::string header = line.substr(0, line.find(':'));
		std::string value = line.substr(line.find(' ')+1);
		headers[header].push_back(value);
	}

	bool chunked = false;
	if (!headers["Transfer-Encoding"].empty())
		if(headers["Transfer-Encoding"][0]=="chunked")
			chunked = true;

	int body_offset = src.find("\r\n\r\n") + 4;

	if (chunked) {
		
		str.seekg(body_offset);
		for (std::string line; safeGetline(str, line);) {
			if (line != "0") {
				if (!line.empty() && (line.find_first_not_of("0123456789abcdefABCDEF") != line.npos)) {
					body.append(line);
				}
			}
		}
	}
	else {
		body = src.substr(body_offset);
	}
}


std::istream& http::Response::safeGetline(std::istream& is, std::string& t)
{
	t.clear();
	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		//case '\n':
			//return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case std::streambuf::traits_type::eof():
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

std::string http::Response::to_string()
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << Status_code() << "\n\r";

	for (auto& headers : headers) {
		for (auto header : headers.second)
			ss << headers.first << ": " << header << "\n\r";
	}
	ss << "\n\r";
	ss << Body();
	return ss.str();
}

int http::Response::Status_code() {
	return status_code;
}
std::string http::Response::Body() {
	return body;
}