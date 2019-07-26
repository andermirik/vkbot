#pragma once
#include "plugin.h"
#include <map>
#include <set>
#include <fstream>

class CallAllPlugin : public Plugin {
public:
	std::map<long long, std::set<long long>> ids;
	CallAllPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area = CallArea::Any);
	void exec(const std::vector<std::string>& args = {}, long peer_id = 0, long from_id = 0);
	void update(std::string text, long peer_id = 0, long from_id = 0);
	std::map<long long, std::set<long long>> restore_session();
	void save_session();
};