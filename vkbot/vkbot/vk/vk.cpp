#include "vk.h"
namespace vk {

	json apisay(std::string text, std::string peer_id, std::string attachment, std::string keyboard) {
		std::string token;
		std::ifstream fin("config/apikey.txt");
		fin >> token;
		fin.close();

		std::string data = "access_token=" + token
			+ "&v=5.80&peer_id=" + peer_id
			+ "&message=" + text
			+ "&attachment=" + attachment
			+ "&keyboard=" + keyboard;
		return json::parse(http::post("https://api.vk.com/method/messages.send", data).Body());
	}

	json get_long_poll_server() {
		std::string token;
		std::ifstream fin("config/apikey.txt");
		fin >> token;
		fin.close();

		auto resp = http::post("https://api.vk.com/method/groups.getLongPollServer", "access_token=" + token + "&v=5.101&group_id=184605473");
		json lpg = json::parse(resp.Body())["response"];
		return lpg;
	}


	std::string failed(json& response, json& lpg, int& ts) {
		if (response.find("failed") != response.end()) {
			if (response["failed"].get<int>() == 1)
				return "failed 1";
			if (response["failed"].get<int>() == 2) {
				lpg = get_long_poll_server();
				return "failed 2";
			}
			if (response["failed"].get<int>() == 3) {
				lpg = get_long_poll_server();
				ts = std::stoi(lpg["ts"].get<std::string>());
				return "failed 3";
			}
		}
		return "";
	}


	std::string upload_pic_to_chat(std::string filename, long long chat_id) {
		std::string token;
		std::ifstream in("config/apikey.txt");
		in >> token;
		in.close();
		auto resp = http::post("https://api.vk.com/method/photos.getMessagesUploadServer", "access_token=" + token + "&v=5.101");
		auto j = json::parse(resp.Body())["response"];
		std::string upload_url = j["upload_url"].get<std::string>();

		std::ifstream fin(filename, std::ios::binary);
		fin.seekg(0, fin.end);
		int length = fin.tellg();
		fin.seekg(0, fin.beg);
		char* buf = new char[length];
		fin.read(buf, length);
		fin.close();

		http::Request req;
		req.method = "POST";
		req.uri = http::Uri(upload_url);
		req.headers["content-type"] = "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW";

		req.body = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n" \
			"Content-Disposition: form-data; name=\"photo\"; filename=\"img.jpg\"\r\n" \
			"Content-Type: image/jpeg\r\n\r\n" + std::string(buf, length) + "\r\n" \
			"------WebKitFormBoundary7MA4YWxkTrZu0gW--";

		resp = http::sendRequest(req);
		j = json::parse(resp.Body());
		resp = http::post("https://api.vk.com/method/photos.saveMessagesPhoto",
			"photo=" + j["photo"].get<std::string>()
			+ "&server=" + std::to_string(j["server"].get<int>())
			+ "&hash=" + j["hash"].get<std::string>()
			+ "&access_token=" + token
			+ "&v=5.101"
		);
		
		j = json::parse(resp.Body());
		if (j.count("error") == 0) {
			j = j["response"];
			return "photo" + std::to_string(j[0]["owner_id"].get<int>()) + "_" + std::to_string(j[0]["id"].get<int>());
		}
		else return "";
	}

	std::string upload_pic_by_url(const std::string& url) {
		std::string token;
		std::ifstream in("config/apikey.txt");
		in >> token;
		in.close();

		auto resp = http::post("https://api.vk.com/method/photos.getMessagesUploadServer", "access_token=" + token + "&v=5.101");
		auto j = json::parse(resp.Body())["response"];
		std::string upload_url = j["upload_url"].get<std::string>();

		auto ret = http::get(url);
		std::string mime = "image/jpeg";
		
		if (url.substr(url.find_last_of('.')+1) == "png")
			mime = "image/png";

		http::Request req;
		req.method = "POST";
		req.uri = http::Uri(upload_url);
		req.headers["content-type"] = "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW";

		req.body = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n" \
			"Content-Disposition: form-data; name=\"photo\"; filename=\"img.jpg\"\r\n" \
			"Content-Type: "+mime+"\r\n\r\n" + ret.Body() + "\r\n" \
			"------WebKitFormBoundary7MA4YWxkTrZu0gW--";

		resp = http::sendRequest(req);
		j = json::parse(resp.Body());
		resp = http::post("https://api.vk.com/method/photos.saveMessagesPhoto",
			"photo=" + j["photo"].get<std::string>()
			+ "&server=" + std::to_string(j["server"].get<int>())
			+ "&hash=" + j["hash"].get<std::string>()
			+ "&access_token=" + token
			+ "&v=5.101"
		);

		j = json::parse(resp.Body());
		if (j.count("error") == 0) {
			j = j["response"];
			return "photo" + std::to_string(j[0]["owner_id"].get<int>()) + "_" + std::to_string(j[0]["id"].get<int>());
		}
		else return "";
	}
}