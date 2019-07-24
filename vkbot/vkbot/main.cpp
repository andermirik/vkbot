#include "http/http.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "windows.h"
#include <set>
#include "psapi.h"
#include <iomanip>
#include "Utils.h"
#include "plugins/plugins.h"

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
			return u8"@isinkin(Синкин) обосрался: " + std::to_string(resp.Status_code());
		}
	}
	return "";
}

json apisay(std::string text, std::string peer_id, std::string attachment="", std::string keyboard = "{\"buttons\":[],\"one_time\":true}") {
	std::string token;
	std::ifstream fin("config/apikey.txt");
	fin >> token;
	fin.close();

	std::string data = "access_token=" + token
		+ "&v=5.80&peer_id=" + peer_id
		+ "&message=" + text
		+ "&attachment=" + attachment
		+"&keyboard="+keyboard;
	return json::parse(http::post("https://api.vk.com/method/messages.send", data).Body());
}

json apisay(const ApiSayObj& api, std::string peer_id) {
	std::string token;
	std::ifstream fin("config/apikey.txt");
	fin >> token;
	fin.close();

	std::string data = "access_token=" + token
		+ "&v=5.80&peer_id=" + peer_id
		+ "&message=" + api.text
		+ "&attachment=" + api.attachment
		+ "&keyboard=" + api.keyboard;
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
	if (response.find("error") != response.end()) {
		if (response["error"].get<int>() == 1)
			return "error 1";
		if (response["error"].get<int>() == 2) {
			lpg = get_long_poll_server();
		}
		if (response["error"].get<int>() == 3) {
			lpg = get_long_poll_server();
			ts = std::stoi(lpg["ts"].get<std::string>());
			return "error 3";
		}
	}
	return "";
}
std::vector<std::string> bot_names = {"ня", "десу", "nya", "desu"};
bool have_bot_name(std::string text) {
	text = to_lower(text);
	auto words = split(text, ' ');
	for (auto& name : bot_names) 
		if (words.size() > 0 && words[0] == name)
			return true;	
	return false;
}

int a_eq_b(const std::vector<std::string>& src, const std::vector<std::string>& sub) {
	int i = 0;
	if (src.size() >= sub.size()) {
		for (i = 0; i < sub.size(); i++) {
			if (src[i] != sub[i])
				if (i < sub.size())
					return 0;
				else
					return i;
		}
	}
	return i;
}

std::string upload_pic_to_chat(std::string filename, long long chat_id) {
	std::string token;
	std::ifstream in("config/apikey.txt");
	in >> token;
	in.close();
	auto resp = http::post("https://api.vk.com/method/photos.getMessagesUploadServer", "access_token=" + token + "&v=5.101");//&peer_id="+std::to_string(chat_id));
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
		"Content-Type: image/jpeg\r\n\r\n"  + std::string(buf, length) + "\r\n" \
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
	j = json::parse(resp.Body())["response"];

	return "photo" + std::to_string(j[0]["owner_id"].get<int>()) + "_" + std::to_string(j[0]["id"].get<int>());
}

void save_session(std::map<long long, std::set<long long>>&ids) {
	std::ofstream fout("ids.txt");
	for (auto& item : ids) {
		fout << item.first<<std::endl;
		for (auto& id : item.second) {
			fout << id<<std::endl;
		}
	}
	fout.close();
}

std::map<long long, std::set<long long>> restore_session() {
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
	return ids;
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

	auto lpg = get_long_poll_server();
	int ts = std::stoi(lpg["ts"].get<std::string>());

	std::map<long long, std::set<long long>> ids = restore_session();	
	std::map<long long, bool> chat_working;
	PluginManager mgr;

	while (true) {
		auto resp = http::post(lpg["server"].get<std::string>(),
			"act=a_check&key="
			+ lpg["key"].get<std::string>()
			+ "&ts=" + std::to_string(ts)
			+"&wait=25"
			);
		json response = json::parse(resp.Body());

		if (auto error = failed(response, lpg, ts) != "") {
			std::cout << "========================" << std::endl;
			std::cout << "Failed with " << error << std::endl
					  << "Trying to continue..." << std::endl;
			std::cout << "========================" << std::endl;
			continue;
		}

		ts = std::stoi(response["ts"].get<std::string>());

		if (ts % 5 == 0) {
			save_session(ids);
			std::cout << "saving..."<<std::endl;
		}
		if (response.find("updates") != response.end())
			for (auto& update : response["updates"])
				if (update["type"].get<std::string>() == "message_new") {
					bool for_me = false;

					std::string text = to_ascii(update["object"]["text"].get<std::string>().c_str());
					long long peer_id = update["object"]["peer_id"].get<long long>();
					long long from_id = update["object"]["from_id"].get<long long>();

					if(peer_id != from_id && from_id > 0)
						ids[peer_id].insert(from_id);

					if (from_id == peer_id)
						std::cout << from_id << " : " << text << std::endl;
					else
						std::cout << from_id << " -> " << peer_id << " : " << text << std::endl;

					if (peer_id < 2000000000 || chat_working[peer_id])
						for_me = true;
					else
						for_me = have_bot_name(text);

					int offset = 0;
					if (for_me == true)
						offset = 1;

					
					/*if (split(text, ' ').size() > 1) {
							if (split(text, ' ')[1] == "режим" && split(text, ' ')[2] == "общения") {
								if (chat_working[peer_id] == false)
									apisay(u8"режим общения включен!", std::to_string(peer_id));
								else
									apisay(u8"режим общения выключен!", std::to_string(peer_id));

								chat_working[peer_id] = !chat_working[peer_id];
								continue;
							}
							if (split(text, ' ')[1] == "позови" && split(text, ' ')[2] == "всех" && from_id == 159334597) {
								std::string call_text = "";
								for (auto&item : ids[peer_id]) {
									call_text += "@id" + std::to_string(item) + " ";
								}	
								apisay(call_text, std::to_string(peer_id));
								continue;
							}
							if (split(text, ' ')[1] == "пиздец" && split(text, ' ')[2] == "позови" && split(text, ' ')[3] == "всех" && from_id == 159334597) {
								auto resp = http::post("https://api.vk.com/method/messages.getConversationMembers",
									"access_token=" + token + "&v=5.101&&peer_id="+std::to_string(peer_id)
								);
								auto j = json::parse(resp.Body())["response"];
								for (auto&item : j["items"]) {
									if(item["member_id"].get<long long>()>0)
										ids[peer_id].insert(item["member_id"].get<long long>());
								}

								std::string call_text = "";
								for (auto&item : ids[peer_id]) {
									call_text += "@id" + std::to_string(item) + " ";
								}
								apisay(call_text, std::to_string(peer_id));
								continue;
							}
						}
						*/
					
					auto words = split(text, ' ');

					bool smth_worked = false;
					for (auto& plugin : mgr.plugins) {
						if (plugin.get()->must_be_private == for_me || plugin.get()->must_be_private == false) {
							int length_command = a_eq_b(std::vector<std::string>(words.begin() + offset, words.end()), plugin.get()->command);
							if (length_command != 0) {
								auto args = std::vector<std::string>(words.begin() + length_command + offset, words.end());
								apisay(plugin.get()->exec(args, peer_id, from_id), std::to_string(peer_id));
								smth_worked = true;
							}
						}
					}
					if (smth_worked == false && (for_me == true || chat_working[peer_id] == true)) {
						apisay(call_sinkin_api(urlencode(to_utf8(text))), std::to_string(peer_id));
					}
				}
				/*
				if (text.find("парни") != std::string::npos)
					apisay(urlencode(u8"парниииииии"), std::to_string(peer_id), "photo-184605473_457239042");
				}
			}*/
	}

	return(0);
}