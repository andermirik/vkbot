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
	Plugin(const std::vector<std::string>& command);
	Plugin();
	std::vector<std::string> command;
	virtual ApiSayObj exec(const std::vector<std::string>& args = {});
};

class FPlugin : public Plugin{
public:
	FPlugin(const std::vector<std::string>& command);
	ApiSayObj exec(const std::vector<std::string>& args = {});
};