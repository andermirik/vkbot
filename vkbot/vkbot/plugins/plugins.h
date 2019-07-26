#pragma once
#include "plugin.h"
#include "plugin_f.h"
#include "plugin_stat.h"
#include "plugin_call_all.h"
#include "plugin_chat_communication.h"
#include "plugin_meme.h"

class PluginManager {
public:
	std::map<std::string, std::unique_ptr<Plugin>> plugins;
	PluginManager() {
		plugins["call_all"] = std::make_unique<CallAllPlugin>(CallAllPlugin({ {"позови", "всех"} ,  {"call", "all"} }, CallArea::Me));
		plugins["default"] = std::make_unique<Plugin>(Plugin({ { "default", "plugin" } }, CallArea::Me));
		plugins["stat"] = std::make_unique<StatPlugin>(StatPlugin({ {"стат"}, {"stat"} }, CallArea::Me));
		plugins["chat_com"] = std::make_unique<ChatCommunicationPlugin>(ChatCommunicationPlugin({ {"режим", "общения"} }, CallArea::Me));
		plugins["F"] =  std::make_unique<FPlugin>(FPlugin({ { "F" } }, CallArea::Any));
		plugins["meme"] = std::make_unique<MemePlugin>(MemePlugin({ { "" } }, CallArea::Any));
	}
};