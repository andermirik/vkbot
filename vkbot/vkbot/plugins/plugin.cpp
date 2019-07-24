#include "plugin.h"

Plugin::Plugin(const std::vector<std::string>& command, bool must_be_private) {
	this->must_be_private = must_be_private;
	this->command = command;
}

Plugin::Plugin() {
}

ApiSayObj Plugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	return ApiSayObj("command: "+join(command, " ") + "\r\n args: " + join(args, " "));
}


