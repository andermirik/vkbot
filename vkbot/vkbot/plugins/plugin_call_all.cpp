#include "plugin_call_all.h"
#include "../http/http.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
CallAllPlugin::CallAllPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {
	ids = restore_session();
}

void CallAllPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	std::string token;
	std::ifstream fin("config/apikey.txt");
	fin >> token;
	fin.close();

	if (args.size() > 0 && (args[0] == "admin" || args[0] == "админ")) {
		auto resp = http::post("https://api.vk.com/method/messages.getConversationMembers",
			"access_token=" + token + "&v=5.101&&peer_id=" + std::to_string(peer_id)
		);
		auto j = json::parse(resp.Body())["response"];
		for (auto&item : j["items"]) {
			if (item["member_id"].get<long long>() > 0)
				ids[peer_id].insert(item["member_id"].get<long long>());
		}
	}

	std::string call_text = "";
	for (auto&item : ids[peer_id]) {
		call_text += "@id" + std::to_string(item) + " ";
	}

	vk::apisay(call_text, std::to_string(peer_id));
}

void CallAllPlugin::update(std::string text, long peer_id, long from_id)
{
	static long long count = 1;

	if (count++ % 5 == 0) {
		save_session();
		std::cout << "saving..." << std::endl;
	}

	if (peer_id != from_id && from_id > 0)
		ids[peer_id].insert(from_id);
}

std::map<long long, std::set<long long>> CallAllPlugin::restore_session()
{
	std::map<long long, std::set<long long>> ids;
	std::ifstream fin("ids.txt");
	std::string s;
	long long current;
	while (std::getline(fin, s)) {
		long long id = std::stoll(s);
		if (id > 2000000000) {
			current = id;
			continue;
		}
		ids[current].insert(id);
	}
	fin.close();
	return ids;
}

void CallAllPlugin::save_session()
{
	std::ofstream fout("ids.txt");
	for (auto& item : ids) {
		fout << item.first << std::endl;
		for (auto& id : item.second) {
			fout << id << std::endl;
		}
	}
	fout.close();
}
