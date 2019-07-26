#include "plugin.h"

bool Plugin::must_be_private()
{
	if(call_area==Any)
		return false;
	else return true;
}

Plugin::Plugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) {
	this->call_area = call_area;
	this->command = command;
}

Plugin::Plugin() {
}

void Plugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	vk::apisay("command: "+join(command[0], " ") + "\r\n args: " + join(args, " "), std::to_string(peer_id));
}

void Plugin::update(std::string text, long peer_id, long from_id)
{
}


