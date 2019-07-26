#include "plugin_f.h"

FPlugin::FPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {
}

void FPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	static std::vector <std::string> F = {
		"photo-184605473_457239023",
		"photo-184605473_457239024",
		"photo-184605473_457239025",
		"photo-184605473_457239026",
		"photo-184605473_457239027",
		"photo-184605473_457239028",
		"photo-184605473_457239029",
		"photo-184605473_457239030",
		"photo-184605473_457239031",
		"photo-184605473_457239040"
	};
	std::string text;
	std::string pict;
	if (from_id > 0) {
		text = "F";
		pict = F[rand() % F.size()];
	}

	vk::apisay(text, std::to_string(peer_id), pict);
}

void FPlugin::update(std::string text, long peer_id, long from_id)
{
}
