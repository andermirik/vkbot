#include "vk.h"
#include <mutex>
namespace vk {

	json apisay(std::string text, std::string peer_id, std::string attachment, std::string keyboard) {
		std::string token = getToken();

		std::string data = "access_token=" + token
			+ "&v=5.80&peer_id=" + peer_id
			+ "&message=" + text
			+ "&attachment=" + attachment
			+ "&keyboard=" + keyboard;
		return json::parse(http::post("https://api.vk.com/method/messages.send", data).body);
	}

	json get_long_poll_server() {
		std::string token = getToken();

		auto resp = http::post("https://api.vk.com/method/groups.getLongPollServer", "access_token=" + token + "&v=5.101&group_id=184605473");
		json lpg = json::parse(resp.body)["response"];
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

	std::string upload_document_by_url(const std::string & url, long peer_id)
	{
		std::string token = getToken();

		auto resp = http::post("https://api.vk.com/method/docs.getMessagesUploadServer",
			"peer_id="+std::to_string(peer_id)+"&type=doc&access_token=" + token + "&v=5.101"
		);
		auto j = json::parse(resp.body)["response"];
		std::string upload_url = j["upload_url"].get<std::string>();
		
		resp = http::upload_files(upload_url, { http::File::from_uri("file", "file.gif", url) });

		j = json::parse(resp.body);
		if (j.count("error") == 0) {
			resp = http::post("https://api.vk.com/method/docs.save",
				"file=" + j["file"].get<std::string>()
				+ "&access_token=" + token
				+ "&v=5.101"
			);

			j = json::parse(resp.body);
			if (j.count("error") == 0) {
				j = j["response"];
				return "doc" + std::to_string(j["doc"]["owner_id"].get<int>()) + "_" + std::to_string(j["doc"]["id"].get<int>());
			}
			else return "";
		}
		else return "";
	}

	std::string upload_pic_by_url(const std::string& url, long peer_id) {
		std::string token = getToken();
		auto resp = http::post("https://api.vk.com/method/photos.getMessagesUploadServer", "peer_id=" + std::to_string(peer_id) + "&access_token=" + token + "&v=5.101");
		auto j = json::parse(resp.body);
		if (j.count("error") != 0) return "";
		j = j["response"];
		std::string upload_url = j["upload_url"].get<std::string>();

		resp = http::upload_files(upload_url, { http::File::from_uri("photo", "img.jpg", url) });

		j = json::parse(resp.body);
		resp = http::post("https://api.vk.com/method/photos.saveMessagesPhoto",
			"photo=" + j["photo"].get<std::string>()
			+ "&server=" + std::to_string(j["server"].get<int>())
			+ "&hash=" + j["hash"].get<std::string>()
			+ "&access_token=" + token
			+ "&v=5.101"
		);

		j = json::parse(resp.body);
		if (j.count("error") == 0) {
			j = j["response"];
			return "photo" + std::to_string(j[0]["owner_id"].get<int>()) + "_" + std::to_string(j[0]["id"].get<int>());
		}
		else return "";
	}

	std::string getToken()
	{
		static std::mutex mutex;
		mutex.lock();
		std::string token;
		std::ifstream in("config/apikey.txt");
		in >> token;
		in.close();
		mutex.unlock();
		return token;
	}
}