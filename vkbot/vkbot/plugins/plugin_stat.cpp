#include "plugin_stat.h"
#include "psapi.h"
#include <iomanip>

StatPlugin::StatPlugin(const std::vector<std::string>& command, bool must_be_private) : Plugin(command, must_be_private) {
}

ApiSayObj StatPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	std::stringstream stat;
	stat << "используется ОЗУ: " << std::setprecision(2) << virtualMemUsedByMe / 1024.0 / 1024.0 << "MB";

	return ApiSayObj(urlencode(to_utf8(stat.str())));
}