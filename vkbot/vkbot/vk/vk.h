#pragma once
#include "../Utils.h"
#include <fstream>
#include "../http/http.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace vk {
	json apisay(std::string text, std::string peer_id, std::string attachment = "", std::string keyboard = "{\"buttons\":[],\"one_time\":true}");
	json get_long_poll_server();
	std::string failed(json& response, json& lpg, int& ts);
	std::string upload_pic_to_chat(std::string filename, long long chat_id);
}