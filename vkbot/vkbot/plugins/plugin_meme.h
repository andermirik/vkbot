#pragma once
#include "plugin.h"

class MemePlugin : public Plugin {
public:
	MemePlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area);
	void exec(const std::vector<std::string>& args = {}, long peer_id = 0, long from_id = 0);
	void update(std::string text, long peer_id = 0, long from_id = 0);
};