#include "plugin_stat.h"
#include "psapi.h"
#include <iomanip>

StatPlugin::StatPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {
}

void StatPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	std::stringstream stat;
	stat << "[ Статистика ]" << std::endl;
	stat << "используется ОЗУ: " << std::setprecision(2) << virtualMemUsedByMe / 1024.0 / 1024.0 << "MB";

	vk::apisay(urlencode(to_utf8(stat.str())), std::to_string(peer_id));
}

void StatPlugin::update(std::string text, long peer_id, long from_id)
{
}
