#include "http/http.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

json apisay(std::string text, std::string peer_id, std::string attachment="", std::string keyboard = "{\"buttons\":[],\"one_time\":true}") {
	std::string token;
	std::ifstream fin("config/apikey.txt");
	fin >> token;
	std::string data = "access_token=" + token
		+ "&v=5.80&peer_id=" + peer_id
		+ "&message=" + text
		+ "&attachment=" + attachment
		+"&keyboard="+keyboard;
	return json::parse(http::post("https://api.vk.com/method/messages.send", data).Body());
}

json get_long_poll_server() {
	std::string token;
	std::ifstream fin("config/apikey.txt");
	fin >> token;

	auto resp = http::post("https://api.vk.com/method/groups.getLongPollServer", "access_token=" + token + "&v=5.101&group_id=184605473");
	json lpg = json::parse(resp.Body())["response"];
	return lpg;
}

std::string failed(json& responce, json& lpg, int& ts) {
	if (responce.find("error") != responce.end()) {
		if (responce["error"].get<int>() == 1)
			return "error 1";
		if (responce["error"].get<int>() == 2) {
			lpg = get_long_poll_server();
		}
		if (responce["error"].get<int>() == 3) {
			lpg = get_long_poll_server();
			ts = std::stoi(lpg["ts"].get<std::string>());
			return "error 3";
		}
	}
	return "";
}

int main() {

	auto lpg = get_long_poll_server();
	int ts = std::stoi(lpg["ts"].get<std::string>());

	while (true) {
		auto resp = http::post(lpg["server"].get<std::string>(),
			"act=a_check&key="
			+ lpg["key"].get<std::string>()
			+ "&ts=" + std::to_string(ts)
			+"&wait=25"
			);
		json response = json::parse(resp.Body());
		ts = std::stoi(response["ts"].get<std::string>());

		if (auto error = failed(response, lpg, ts) != "") {
			std::cout << "========================" << std::endl;
			std::cout << "Failed with " << error << std::endl
					  << "Trying to continue..." << std::endl;
			std::cout << "========================" << std::endl;
			continue;
		}

		for (auto& update : response["updates"])
				if (update["type"].get<std::string>() == "message_new") {
					std::string text = update["object"]["text"].get<std::string>();
					long long peer_id = update["object"]["peer_id"].get<long long>();
					std::cout << peer_id << " : " << text << std::endl;
					apisay(text, std::to_string(peer_id));
				}
	}

	return(0);
}