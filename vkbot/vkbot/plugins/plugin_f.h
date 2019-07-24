#pragma once
#include "plugin.h"
class FPlugin : public Plugin {
public:
	FPlugin(const std::vector<std::string>& command, bool must_be_private = false);
	ApiSayObj exec(const std::vector<std::string>& args = {}, long peer_id = 0, long from_id = 0);
};