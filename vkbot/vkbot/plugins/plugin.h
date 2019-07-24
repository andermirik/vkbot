#pragma once
#include <string>
#include <vector>
#include "../Utils.h"

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

class Plugin {
public:
	std::vector<std::string> command; 
	bool must_be_private;
	Plugin(const std::vector<std::string>& command, bool must_be_private = false);
	Plugin();
	virtual ApiSayObj exec(const std::vector<std::string>& args = {}, long peer_id=0, long from_id=0);
};

