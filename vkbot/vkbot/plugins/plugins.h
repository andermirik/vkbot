#pragma once
#include "plugin.h"
#include "plugin_f.h"
#include "plugin_stat.h"

class PluginManager {
public:
	std::vector<std::unique_ptr<Plugin>> plugins;
	PluginManager() {
		plugins.push_back(std::make_unique<Plugin>(Plugin({ "default", "plugin" }, true)));
		plugins.push_back(std::make_unique<StatPlugin>(StatPlugin({ "status" }, true)));
		plugins.push_back(std::make_unique<FPlugin>(FPlugin({ "F" }, false)));
	}
};