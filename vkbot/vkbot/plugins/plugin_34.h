#pragma once
#include "plugin.h"

class Rule34Plugin : public Plugin {
public:
	std::map<long long, int> last_call;
	Rule34Plugin(const std::vector<std::vector<std::string>>& command, CallArea call_area = CallArea::Any);
	void exec(const std::vector<std::string>& args = {}, long peer_id = 0, long from_id = 0);
	void update(std::string text, long peer_id = 0, long from_id = 0);
};