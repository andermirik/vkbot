#pragma once
#include "uri.h"
#include "request.h"
#include "response.h"

namespace http {
	typedef std::map<std::string, std::string> Table;

	class File{
	public:
		std::string propertyname;
		std::string filename;
		std::string filecontent;

		static File from_filename(std::string const& propertyname, std::string const& filename);
		static File from_uri     (std::string const& propertyname, std::string const& filename, std::string const & path);
		static File from_memory  (std::string const& propertyname, std::string const& filename, std::string const& src);
	};

	Response get(std::string const& path, std::string const& data = "",  Table const& headers = Table());
	Response post(std::string const& path, std::string const& data = "", Table const& headers = Table());

	Response upload_files(std::string const& path, std::vector<File> const& files, Table const& headers = Table());
	std::string get_mime(std::string const& filename);
	
	Response sendRequest(Request&request);
	Response sendHTTPS(Request& request);
	Response sendHTTP(Request& request);

}
namespace https = http;