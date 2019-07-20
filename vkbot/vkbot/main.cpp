#include "http/http.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "TCHAR.H"
#include <string>
#include "windows.h"
#include <codecvt>

using namespace std::string_literals;

int sinkinTest(int count) {
	int res = 0;
	for (int i = 0; i < count; i++) {
		auto resp = http::get("https://isinkin-bot-api.herokuapp.com/1/talk?q=привет");
		std::cout << i << std::endl;
		if (resp.Status_code() == 200)
			res++;
	}
	return res;
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

using namespace std::string_literals;
std::string to_utf8(const std::string& str, const std::locale& loc = std::locale{}) {
	using wcvt = std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t>;
	std::u32string wstr(str.size(), U'\0');
	std::use_facet<std::ctype<char32_t>>(loc).widen(str.data(), str.data() + str.size(), &wstr[0]);
	return wcvt{}.to_bytes(
		reinterpret_cast<const int32_t*>(wstr.data()),
		reinterpret_cast<const int32_t*>(wstr.data() + wstr.size())
	);
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
	system("chcp 1251");
	system("cls");
	setlocale(LC_ALL, "rus");

	//std::cout << sinkinTest(15)<<std::endl;

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
		bool for_me = false;
		int length_words = 0;
		for (auto& update : response["updates"])
				if (update["type"].get<std::string>() == "message_new") {
					std::string text = to_ascii(update["object"]["text"].get<std::string>().c_str());
					long long peer_id = update["object"]["peer_id"].get<long long>();

					std::cout << peer_id << " : " <<  text << std::endl;
					auto words = split(text, ' ');
					for (auto word : words)
						if (word == "ня" || word == "десу" || word == "nya" || word == "desu") {
							for_me = true;
						}
					
					if (for_me) {
						std::cout << text.substr(length_words) << std::endl;
						std::cout << "https://isinkin-bot-api.herokuapp.com/1/talk?q=" + update["object"]["text"].get<std::string>() << std::endl;
						auto resp = http::get("https://isinkin-bot-api.herokuapp.com/1/talk?q="+text);
						if (resp.Status_code() == 200) {
							std::cout << text.substr(length_words);
							std::string answer = json::parse(resp.Body())["text"].get<std::string>();
							apisay(answer, std::to_string(peer_id));
						}
						else {
							apisay(u8"Синкин обосрался: " + std::to_string(resp.Status_code()), std::to_string(peer_id));
						}
					}
				}
	}

	return(0);
}