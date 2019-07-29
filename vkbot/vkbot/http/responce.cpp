#include "responce.h"
#include <sstream>

http::Response::Response() {
	body = "";
}

http::Response::Response(std::string str) {
	fromString(str);
}

void http::Response::fromString(std::string str) {
	int pos = 0;

	std::string http_version = readUntilChr(str, ' ', pos);
	this->status_code = std::stoi(readUntilChr(str, ' ', pos));
	std::string res = readUntilChr(str, '\n', pos);

	std::string row = readUntilChr(str, '\n', pos);

	while (row != "\r") {
		int row_pos = 0;
		std::string header = readUntilChr(row, ':', row_pos);
		std::string value = readUntilChr(row, '\r', ++row_pos);

		headers[header].push_back(value);
		row = readUntilChr(str, '\n', pos);
	}

	bool chunked = false;
	if (!headers["Transfer-Encoding"].empty())
		if(headers["Transfer-Encoding"][0]=="chunked")
			chunked = true;

	std::stringstream ss;
	std::stringstream src(str.substr(pos));
	if (chunked) {
		int i = 0;
		for (std::string line; safeGetline(src, line);){
			i++;
			if (line != "0") {
				if (!line.empty() && (line.find_first_not_of("0123456789abcdefABCDEF") != line.npos)) {
					ss << line;
				}
			}
		}
	}
	else {
		for (pos; pos < str.size(); pos++) {
			ss << str[pos];
		}
	}

	body = ss.str();
}

std::string http::Response::readUntilChr(std::string& src, char until, int& pos) {
	std::stringstream ss;
	while (src[pos] != until && pos < src.size()) {
		ss << src[pos++];
	}
	pos++;
	return ss.str();
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