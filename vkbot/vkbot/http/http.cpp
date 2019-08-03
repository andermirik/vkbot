#include "winsock2.h"
#include "windows.h"
#include "ws2tcpip.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include "http.h"
#include "mime.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>

namespace http {

	Response get(std::string const& path, std::string const& data, Table const& headers) {
		Request request;
		request.method = "GET";
		request.uri = Uri(path);
		request.body = data;
		request.headers = headers;
		return sendRequest(request);
	}

	Response post(std::string const& path, std::string const& data, Table const& headers) {
		Request request;
		request.method = "POST";
		request.uri = Uri(path);
		request.body = data;
		request.headers = headers;
		return sendRequest(request);
	}

	Response upload_files(std::string const & path, std::vector<File> const& files, Table const& headers)
	{
		static std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";

		Request request;
		request.method = "POST";
		request.uri = Uri(path);
		request.headers = headers;

		request.headers["content-type"] = "multipart/form-data; boundary=" + boundary;

		for (auto& file : files) {
			request.body.append("--"+boundary+"\r\n");
			request.body.append("Content-Disposition: form-data; name=\"" + file.propertyname + "\";");
			request.body.append(file.filename == "" ? "\r\n" : " filename=\"" + file.filename + "\";\r\n");
			request.body.append(file.filename == "" ? "\r\n\r\n" : "Content-Type: " + get_mime(file.filename) + "\r\n\r\n");
			request.body.append(file.filecontent);
			request.body.append("\r\n");
		}
		if (!files.empty()) {
			request.body.append("--" + boundary + "--");
		}
		return sendRequest(request);
	}

	std::string get_mime(std::string const & filename)
	{
		std::string type = filename.substr(filename.find_last_of('.'));
		if(types::mime.find(type)!=types::mime.end())
			return types::mime[type];
		return "image/jpeg";
	}

	Response sendRequest(Request&request) {
		Response resp;
		if (request.uri.scheme == "http") 
			resp = sendHTTP(request);
		else if (request.uri.scheme == "https") 
			resp = sendHTTPS(request);
		return resp;
	}

	Response sendHTTPS(Request& request) {
		WSADATA wsaData;
		WORD DllVersion = MAKEWORD(2, 2);
		if (WSAStartup(DllVersion, &wsaData)) {
			return Response();
		}
		std::string req = request.to_string();
		addrinfo* pAddrInfo;
		getaddrinfo(request.uri.host.c_str(), "443", 0, &pAddrInfo);

		SOCKET connection = socket(
			pAddrInfo->ai_family,
			pAddrInfo->ai_socktype,
			pAddrInfo->ai_protocol
		);
		if (!connection)
			return Response();

		int error = connect(connection, pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
		if (error)
			return Response();

		SSL_library_init();
		SSLeay_add_ssl_algorithms();
		SSL_load_error_strings();
		
		const SSL_METHOD *meth = TLSv1_2_client_method();
		SSL_CTX *ctx = SSL_CTX_new(meth);

		SSL *ssl = SSL_new(ctx);
		if (!ssl)
			return Response();

		int sock = SSL_get_fd(ssl);
		SSL_set_fd(ssl, connection);

		error = SSL_connect(ssl);
		if (error <= 0)
			return Response();

		int len = SSL_write(ssl, req.c_str(), req.size());
		if (len <= 0)
			return Response();

		std::string result = "";
		char buffer[16384] = { 0 };//16 kib
		int bytes_recv = 0;

		do {
			bytes_recv = SSL_read(ssl, buffer, sizeof(buffer));
			if (bytes_recv > 0)
				result.append(buffer, bytes_recv);
		} while (bytes_recv > 0);

		closesocket(connection);
		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);

		return Response(result);
	}
	Response sendHTTP(Request& request) {
		WSADATA wsaData;
		WORD DllVersion = MAKEWORD(2, 2);
		if (WSAStartup(DllVersion, &wsaData)) {
			return Response();
		}

		std::string req = request.to_string();

		addrinfo* pAddrInfo;
		getaddrinfo(request.uri.host.c_str(), "80", 0, &pAddrInfo);

		SOCKET connection = socket(
			pAddrInfo->ai_family,
			pAddrInfo->ai_socktype,
			pAddrInfo->ai_protocol
		);
		if (!connection)
			return Response();

		int error = connect(connection, pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
		if (error)
			return Response();

		int len = send(connection, req.c_str(), req.size(), 0);
		if (len <= 0)
			return Response();

		std::string result = "";
		char buffer[16384] = { 0 };//16 kib
		int bytes_recv = 0;

		do {
			bytes_recv = recv(connection, buffer, sizeof(buffer), 0);
			if (bytes_recv > 0)
				result.append(buffer, bytes_recv);
		} while (bytes_recv > 0);

		closesocket(connection);
		return Response(result);
	}
	
	File File::from_filename(std::string const & propertyname, std::string const & filename)
	{
		std::ifstream fin;
		fin.exceptions(std::ios::failbit | std::ios::badbit);
		fin.open(filename, std::ios::binary);

		std::stringstream ss;
		ss << fin.rdbuf();
		
		File result;
		result.filename = filename;
		result.propertyname = propertyname;
		result.filecontent = ss.str();

		return result;
	}

	File File::from_uri(std::string const & propertyname, std::string const &  filename,  std::string const & path)
	{
		File result;

		result.filecontent = http::get(path).body;
		result.propertyname = propertyname;
		result.filename = filename;

		return result;
	}

	File File::from_memory(std::string const & propertyname, std::string const & filename, std::string const & src)
	{
		File result;
		result.propertyname = propertyname;
		result.filename = filename;
		result.filecontent = src;
		return result;
	}

}