#pragma once
#include "plugin.h"

class RandCatsPlugin : public Plugin {
public:
	RandCatsPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area = CallArea::Any);
	void exec(const std::vector<std::string>& args = {}, long peer_id = 0, long from_id = 0);
	void update(std::string text, long peer_id = 0, long from_id = 0);
};