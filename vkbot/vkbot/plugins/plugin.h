#pragma once
#include <string>
#include <vector>
#include "../Utils.h"
#include "../vk/vk.h"

class ApiSayObj {
public:
	std::string text;
	std::string attachment;
	std::string keyboard; 
	ApiSayObj() {
		text = "";
		attachment = "";
		keyboard = "{\"buttons\":[],\"one_time\":true}";
	}
	ApiSayObj(std::string text, std::string attachment = "", std::string keyboard = "{\"buttons\":[],\"one_time\":true}") {
		this->text = text;
		this->attachment = attachment;
		this->keyboard = keyboard;
	}
};

enum CallArea {Any, Me};
class Plugin {
public:
	std::vector<std::vector<std::string>> command;
	CallArea call_area;
	bool must_be_private();
	Plugin(const std::vector<std::vector<std::string>>& command, CallArea call_area = CallArea::Any);
	Plugin();
	virtual void exec(const std::vector<std::string>& args = {}, long peer_id=0, long from_id=0);
	virtual void update(std::string text, long peer_id = 0, long from_id = 0);
};

