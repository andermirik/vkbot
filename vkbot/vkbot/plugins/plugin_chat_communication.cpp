#include "plugin_chat_communication.h"

ChatCommunicationPlugin::ChatCommunicationPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {

}

void ChatCommunicationPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	std::string out_text;
	if (chat_communication[peer_id] == false)
		out_text = u8"режим общения включен!";
	else
		out_text = u8"режим общения выключен!";

	chat_communication[peer_id] = !chat_communication[peer_id];

	vk::apisay(out_text, std::to_string(peer_id));
}

void ChatCommunicationPlugin::update(std::string text, long peer_id, long from_id)
{
}
