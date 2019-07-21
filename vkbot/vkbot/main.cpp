#include "http/http.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "TCHAR.H"
#include <string>
#include "windows.h"
#include <codecvt>
#include <set>



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

std::vector<std::string> split(const std::string& s, char seperator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
	return output;
}

std::string char2hex(const char dec) {
	char dig1 = (dec & 0xF0) >> 4;
	char dig2 = (dec & 0x0F);

	if (0 <= dig1 && dig1 <= 9) dig1 += 48;
	if (10 <= dig1 && dig1 <= 15) dig1 += 87;
	if (0 <= dig2 && dig2 <= 9) dig2 += 48;
	if (10 <= dig2 && dig2 <= 15) dig2 += 87;

	std::string r;
	r.append(&dig1, 1);
	r.append(&dig2, 1);
	return r;
}

static void cp1251_to_utf8(char *out, const char *in) {
	static const int table[128] = {
		0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
		0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
		0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
		0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
		0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
		0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
		0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
		0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
		0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
		0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
		0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
		0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
		0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
		0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
		0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
		0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
	};
	while (*in)
		if (*in & 0x80) {
			int v = table[(int)(0x7f & *in++)];
			if (!v)
				continue;
			*out++ = (char)v;
			*out++ = (char)(v >> 8);
			if (v >>= 16)
				*out++ = (char)v;
		}
		else
			*out++ = *in++;
	*out = 0;
}
std::string to_utf8(const std::string& str) {
	char*out = new char[3*str.size()+1];
	cp1251_to_utf8(out, str.c_str());
	return std::string(out);
}

std::string to_ascii(std::string utf8s) {
	const char* utf8 = utf8s.c_str();
	int length = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, -1, NULL, 0);
	if (length > 0)
	{
		wchar_t* wide = new wchar_t[length];
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, -1, wide, length);
		size_t convertedChars = 0;
		char* ansi = new char[length];
		wcstombs_s(&convertedChars, ansi, length, wide, _TRUNCATE);
		return std::string(ansi);
	}
	return "";
}

std::string urlencode(const std::string &url) {

	std::string escaped;
	for (const char& c : url) {
		if ((48 <= c && c <= 57) ||
			(65 <= c && c <= 90) ||
			(97 <= c && c <= 122) ||
			(c == '~' || c == '!' || c == '*' || c == '(' || c == ')' || c == '\'')
			) {
			escaped.append(&c, 1);
		}
		else {
			escaped.append("%");
			escaped.append(char2hex(c));
		}
	}

	return escaped;
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
								if(chat_working[peer_id]==false)
									apisay(u8"режим общения включен!", std::to_string(peer_id));
								else 
									apisay(u8"режим общения выключен!", std::to_string(peer_id));

								chat_working[peer_id] = !chat_working[peer_id];
								continue;
							}
							if (split(text, ' ')[1] == "позови" && split(text, ' ')[2] == "всех" && from_id == 159334597) {
								std::string call_text = "";
								for (auto&item : ids[peer_id]) {
									call_text += "@id"+std::to_string(item) + " ";
								}
								apisay(call_text, std::to_string(peer_id));
								continue;
							}
						}
						apisay(call_sinkin_api(urlencode(to_utf8(text))), std::to_string(peer_id));
					}
					else {
						if (text == "F")
							apisay("F", std::to_string(peer_id));
						if (text == "привет")
							apisay(call_sinkin_api(urlencode(to_utf8("привет"))), std::to_string(peer_id));
						if (text == "работает")
							apisay(to_utf8("не работает!))"), std::to_string(peer_id));
					}
				}
	}

	return(0);
}