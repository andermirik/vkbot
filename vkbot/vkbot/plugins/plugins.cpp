#include "plugins.h"

Plugin::Plugin(const std::vector<std::string>& command)
{
	this->command = command;
}

Plugin::Plugin()
{
}

ApiSayObj Plugin::exec(const std::vector<std::string>& args)
{
	return ApiSayObj(join(command, " ") + ": " + join(args, " "));
}

FPlugin::FPlugin(const std::vector<std::string>& command) : Plugin(command) {
}
ApiSayObj FPlugin::exec(const std::vector<std::string>& args)
{
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
	return ApiSayObj("F", F[rand() % F.size()]);
}
