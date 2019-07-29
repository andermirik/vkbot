#include "http/http.h"
#include <nlohmann/json.hpp>

#include "plugins/plugins.h"
#include "Utils.h"

#include "windows.h"

#include <iomanip>
#include <thread>

using namespace std::string_literals;
using json = nlohmann::json;

std::string call_sinkin_api(std::string text) {
	if (text != "" && text!="%5c") {
		auto resp = http::get("https://isinkin-bot-api.herokuapp.com/1/talk?q=" + text);
		if (resp.Status_code() == 200) {
			std::string answer = json::parse(resp.Body())["text"].get<std::string>();
			return answer;
		}
		else {
			std::cout << text<<std::endl;
			return u8"@isinkin(������) ���������: " + std::to_string(resp.Status_code());
		}
	}
	return "";
}

std::vector<std::string> bot_names = {"��", "����", "nya", "desu", "[club184605473|@nyadesubot]"};

bool have_bot_name(std::string text) {
	text = to_lower(text);
	auto words = split(text, ' ');
	for (auto& name : bot_names) 
		if (words.size() > 0 && words[0] == name)
			return true;	
	return false;
}

int main() {
	system("chcp 1251");
	system("cls");
	setlocale(LC_ALL, "rus");
	srand(time(0));

	std::string token;
	std::ifstream fin("config/apikey.txt");
	fin >> token;
	fin.close();

	auto lpg = vk::get_long_poll_server();
	int ts = std::stoi(lpg["ts"].get<std::string>());

	PluginManager mgr;
	
	while (true) {
		auto resp = http::post(lpg["server"].get<std::string>(),
			"act=a_check&key="
			+ lpg["key"].get<std::string>()
			+ "&ts=" + std::to_string(ts)
			+"&wait=25"
			);
		if (resp.Body() == "")
			continue;

		json response = json::parse(resp.Body());
		
		std::string error = vk::failed(response, lpg, ts);
		if (error != "") {
			std::cout << "========================" << std::endl;
			std::cout << "Failed with " << error << std::endl
					  << "Trying to continue..." << std::endl;
			std::cout << "========================" << std::endl;
			continue;
		}

		ts = std::stoi(response["ts"].get<std::string>());

		if (response.find("updates") != response.end())
			for (auto& update : response["updates"]) {
				if (update["type"].get<std::string>() == "message_new") {
					bool for_me = false;

					std::string text = to_ascii(update["object"]["text"].get<std::string>().c_str());
					long long peer_id = update["object"]["peer_id"].get<long long>();
					long long from_id = update["object"]["from_id"].get<long long>();

					if (from_id == peer_id)
						std::cout << from_id << " : " << text << std::endl;
					else
						std::cout << from_id << " -> " << peer_id << " : " << text << std::endl;

					if (peer_id < 2000000000)
						for_me = true;
					else
						for_me = have_bot_name(text);

					int offset = 0;
					if (for_me == true)
						offset = 1;

					auto words = split(text, ' ');

					bool smth_worked = false;
					for (auto& plugin : mgr.plugins) {
						if (plugin.second.get()->must_be_private() == for_me || plugin.second.get()->must_be_private() == false) {
							for (auto& command : plugin.second.get()->command) {
 								int length_command = a_in_b_overlay(std::vector<std::string>(words.begin() + offset, words.end()), command);
								if (length_command != 0) {
									auto args = std::vector<std::string>(words.begin() + length_command + offset, words.end());
									if (!update["object"]["attachments"].empty()) {
										args.push_back("attachments");
										args.push_back(update["object"]["attachments"].dump());
									}
									plugin.second.get()->exec(args, peer_id, from_id);
									smth_worked = true;
								}
							}
						}
						plugin.second.get()->update(text, peer_id, from_id);
					}
					if (smth_worked == false && (for_me == true || ((ChatCommunicationPlugin*)mgr.plugins["chat_com"].get())->chat_communication[peer_id] == true)) {
						vk::apisay(call_sinkin_api(urlencode(to_utf8(text))), std::to_string(peer_id));
					}
				}
			}
	}
	return(0);
}
