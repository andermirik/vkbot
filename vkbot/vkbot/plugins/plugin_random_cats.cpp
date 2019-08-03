#include "plugin_random_cats.h"
#include "../http/http.h"


using json = nlohmann::json;
RandCatsPlugin::RandCatsPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {

}

void RandCatsPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	auto url = json::parse(http::get("https://aws.random.cat/meow").body)["file"];
	//gif
	std::string pic = vk::upload_pic_by_url(url.get < std::string > (), from_id);
	vk::apisay("", std::to_string(peer_id), pic);
}

void RandCatsPlugin::update(std::string text, long peer_id, long from_id)
{

}
