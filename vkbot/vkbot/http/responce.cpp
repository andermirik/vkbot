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

	std::stringstream ss;
	for (pos; pos < str.size(); pos++) {
		ss << str[pos];
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