#include "plugin_f.h"

FPlugin::FPlugin(const std::vector<std::string>& command, bool must_be_private) : Plugin(command, must_be_private) {
}

ApiSayObj FPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
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
	return ApiSayObj(text, pict);
}