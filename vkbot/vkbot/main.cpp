#include "http/http.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
int main() {
	//184605473

	std::string token;
	std::ifstream fin("config/apikey.txt");
	fin >> token;

	while (true) {
		
		auto a = http::post("https://api.vk.com/method/groups.getLongPollServer?access_token="+token+"&v=5.101&group_id=184605473");
	}

	return(0);
}