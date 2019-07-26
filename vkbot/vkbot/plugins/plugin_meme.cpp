#include "plugin_meme.h"


MemePlugin::MemePlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {
}

void MemePlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {

}

void MemePlugin::update(std::string text, long peer_id, long from_id)
{
	if (text.find("парни") != std::string::npos)
		vk::apisay(urlencode(u8"парниииииии"), std::to_string(peer_id), "photo-184605473_457239042");
}
