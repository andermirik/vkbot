#include "http/http.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "windows.h"
#include <set>
#include "psapi.h"
#include <iomanip>

#include "Utils.h"

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
	for (int i = 0; i < text.size();i++) {
		text[i] = std::tolower(text[i]);
	}
	for (auto& word : split(text, ' ')) {
		for(auto& name: bot_names)
			if (word == name) {
				return true;
			}
	}
	return false;
}

std::string call_all() {
	return "";
}

std::string send_pic_to_chat(std::string filename, long long chat_id) {
	std::string token;
	std::ifstream in("config/apikey.txt");
	in >> token;
	auto resp = http::post("https://api.vk.com/method/photos.getMessagesUploadServer", "access_token=" + token + "&v=5.101");//&peer_id="+std::to_string(chat_id));
	auto j = json::parse(resp.Body())["response"];
	std::string upload_url = j["upload_url"].get<std::string>();

	std::ifstream fin(filename, std::ios::binary);
	fin.seekg(0, fin.end);
	int length = fin.tellg();
	fin.seekg(0, fin.beg);
	char* buf = new char[length];
	fin.read(buf, length);

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
	std::cout << j;
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

	auto lpg = get_long_poll_server();
	int ts = std::stoi(lpg["ts"].get<std::string>());

	std::map<long long, std::set<long long>> ids = restore_session();	
	std::map<long long, bool> chat_working;

	

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

					if (for_me) {
						if (split(text, ' ').size() > 1) {
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
						}
						int num = 0;
						std::string url;
						for (auto& size : update["object"]["attachments"][0]["photo"]["sizes"])
							if (size["width"].get<int>() > num) {
								num = size["width"].get<int>();
								url = size["url"].get<std::string>();
							}
						auto pic = http::get(url).Body();

						apisay(call_sinkin_api(urlencode(to_utf8(text))), std::to_string(peer_id));
					}
					else {
						if (text == "F"  && from_id > 0) {
							static std::vector <std::string> F = {
							"photo-184605473_457239023",
							"photo-184605473_457239024",
							"photo-184605473_457239025",
							"photo-184605473_457239026",
							"photo-184605473_457239027",
							"photo-184605473_457239028",
							"photo-184605473_457239029",
							"photo-184605473_457239030",
							"photo-184605473_457239031"
							};
							apisay("F", std::to_string(peer_id), F[rand() % F.size()]);
						}
						if (text == "картинка") {
							apisay(urlencode(u8"картинка"), std::to_string(peer_id), send_pic_to_chat("img2.jpg", peer_id));
						}
						if (text == "привет")
							apisay(call_sinkin_api(urlencode(to_utf8("привет"))), std::to_string(peer_id));
						if (text == "работает")
							apisay(to_utf8("не работает!))"), std::to_string(peer_id));
						if (text == "сколько") {
							PROCESS_MEMORY_COUNTERS_EX pmc;
							GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
							SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
							std::stringstream memory;
							memory << std::setprecision(2) << virtualMemUsedByMe / 1024.0 / 1024.0 << "MB";
							apisay(memory.str(), std::to_string(peer_id));
						}
					}
				}
	}

	return(0);
}