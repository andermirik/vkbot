#include <xml2json.hpp>
#include "plugin_34.h"
#include "../http/http.h"
#include "nlohmann/json.hpp"
#include <thread>

using json = nlohmann::json;
Rule34Plugin::Rule34Plugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {

}

void Rule34Plugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	if (from_id != 159334597 && from_id != 203053340) {
		vk::apisay(urlencode(u8"ток для админов"), std::to_string(peer_id));
		return;
	}

	int offset = 0;
	int count_pic = 1;
	if (args.size() >= 1 && args[0].find_first_not_of("0123456789")==args[0].npos) {
		count_pic = std::stoi(args[0]);
		offset = 1;
	}

	vk::apisay(urlencode(u8"начинаю качать фотки..."), std::to_string(peer_id));
	static std::string black_list = "-anthro+-fur+-scat*+-darling_in_the_franxx+-furry+-dragon+-guro+-animal_penis+-animal+-wolf+-fox+-webm+-gif+-my_little_pony+-monster*+-3d+-animal*+-ant+-insects+-mammal+-horse+-blotch+-deer+-real*+-shit+-everlasting_summer+-copro*+-wtf+";
	auto ret = http::get("https://rule34.xxx/index.php?page=dapi&q=index&s=post&tags="
		+ black_list + join(std::vector<std::string>(args.begin()+offset, args.end()), "+") + "&limit=10000"
	);
	auto xml = xml2json(ret.Body().c_str());
	auto posts = json::parse(xml)["posts"]["post"];

	std::vector<std::string> images;
	std::vector<std::thread> thread_pool;
	int k = 0;

	for (int i = 1; i <= count_pic; i++) {
		auto post = posts[rand() % (posts.size() - 1)];

		thread_pool.push_back(std::thread([&images, post, &k]() {
			images.push_back(vk::upload_pic_by_url(post["@file_url"].get<std::string>()));
			std::cout << ++k << " done.\n";
		}));

		if (i % 10 == 0 || i == count_pic) {
			for (auto& thread : thread_pool)
				thread.join();

			thread_pool.clear();
			vk::apisay("", std::to_string(peer_id), join(images, ","));
			images.clear();

			if (i == count_pic) {
				break;
			}
		}
	}
}

void Rule34Plugin::update(std::string text, long peer_id, long from_id)
{
	
}
