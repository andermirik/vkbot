#include "winsock2.h"
#include "windows.h"
#include "ws2tcpip.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

#include "http.h"

#include <iostream>
#include <sstream>



namespace http {


	Response get(std::string const& path, std::string const& data) {
		Request request;
		request.method = "GET";
		request.uri = Uri(path);
		request.body = data;
		return sendRequest(request);
	}

	Response post(std::string const& path, std::string const& data) {
		Request request;
		request.method = "POST";
		request.uri = Uri(path);
		request.body = data;
		return sendRequest(request);
	}

	Response sendRequest(Request&request) {
		Response resp;
		if (request.uri.scheme() == "http") 
			resp = sendHTTP(request);
		else if (request.uri.scheme() == "https") 
			resp = sendHTTPS(request);
		return resp;
	}

	Response sendHTTPS(Request& request) {
		WSADATA wsaData;
		WORD DllVersion = MAKEWORD(2, 2);
		if (WSAStartup(DllVersion, &wsaData)) {
			return Response();
		}
		std::string req = str(request);
		addrinfo* pAddrInfo;
		getaddrinfo(request.uri.host().c_str(), "443", 0, &pAddrInfo);

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

		std::string req = str(request);

		addrinfo* pAddrInfo;
		getaddrinfo(request.uri.host().c_str(), "80", 0, &pAddrInfo);

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

	std::string str(Request& request) {
		std::stringstream ss;
		ss << request.method << " /" << request.uri.path() + "?" + request.uri.args() << " HTTP/1.1\r\n";
		ss << "Host: " << request.uri.host() << "\r\n";
		ss << "Content-Length: " << request.body.size() << "\r\n";
		ss << "Connection: close\r\n";
		ss << "Accept-Encoding: identity\r\n";
		for (auto header : request.headers) {
			ss << header.first << ": " << header.second << "\r\n";
		}
		ss << "\r\n";
		ss << request.body;
		return ss.str();
	}
	std::string str(Response& responce) {
		std::stringstream ss;
		ss << "HTTP/1.1 " << responce.Status_code() << "\n\r";

		for (auto& headers : responce.headers) {
			for (auto header : headers.second)
				ss << headers.first << ": " << header << "\n\r";
		}
		ss << "\n\r";
		ss << responce.Body();
		return ss.str();
	}
}