#include "plugin_34.h"
#include "xml2json.hpp"
#include "../http/http.h"
#include <thread>
#include <mutex>

using json = nlohmann::json;
Rule34Plugin::Rule34Plugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {
	
}

void Rule34Plugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	srand(time(0));
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);

	bool admin = true;
	if (from_id != 159334597 && from_id != 203053340) {
		admin = false;
	}

	if (last_call.find(peer_id) == last_call.end())
		last_call[peer_id] = 0;

	int offset = 0;
	int count_pic = 1;
	if (args.size() >= 1 && args[0].find_first_not_of("0123456789")==args[0].npos) {
		count_pic = std::stoi(args[0]);
		offset = 1;
	}

	if (!admin && count_pic > 10) {
		count_pic = 10;
	}
	std::cout << time(0) - last_call[peer_id] << std::endl;
	if (!admin && time(0) - last_call[peer_id] <= 15) {
		vk::apisay(urlencode(u8"перезарядка r34: " + std::to_string(15 - (time(0) - last_call[peer_id])) + u8" секунд"), std::to_string(peer_id));	
		return;
	}

	static std::string black_list = "-anthro+-fur+-scat*+-furry+-dragon+-guro+-animal_penis+-animal+-wolf+-fox+-webm+-my_little_pony+-monster*+-3d+-animal*+-ant+-insects+-mammal+-horse+-blotch+-deer+-real*+-shit+-copro*+-wtf+";
	auto ret = http::get("https://rule34.xxx/index.php?page=dapi&q=index&s=post&tags="
		+ black_list + join(std::vector<std::string>(args.begin()+offset, args.end()), "+") + "&limit=10000"
	);

	auto xml = xml2json(ret.body.c_str());
	auto posts = json::parse(xml)["posts"]["post"];

	std::vector<std::string> images;
	std::vector<std::thread> thread_pool;
	int k = 0;

	if (posts.size() == 0) {
		vk::apisay(urlencode(u8"ничего не нашел!"), std::to_string(peer_id));
		last_call[peer_id] = 0;
		return;
	}
	else if (posts.size() < count_pic)
		count_pic = posts.size();

	vk::apisay(urlencode(u8"начинаю качать фотки..."), std::to_string(peer_id));

	for (int i = 1; i <= count_pic; i++) {
		auto post = posts[rand() % (posts.size() - 1)];
		
		std::string url = post["@file_url"].get<std::string>();

		thread_pool.push_back(std::thread([&images, url, &k, from_id]() {
			static std::mutex mutex_images;
			std::string format = url.substr(url.find_last_of('.') + 1);
			std::string image;

			if (format != "webm") {
				if (format != "gif") 
					image = vk::upload_pic_by_url(url, from_id);
				else
					image = vk::upload_document_by_url(url, from_id);

				if (image != "") {
					mutex_images.lock();
					images.push_back(image);
					mutex_images.unlock();
				}
			}
			std::cout << ++k << " done. " << url << std::endl;
		}));

		if (i % 10 == 0 || i == count_pic) {
			for (auto& thread : thread_pool)
				thread.join();

			thread_pool.clear();
			std::cout << vk::apisay("", std::to_string(peer_id), join(images, ",")) << std::endl;
			images.clear();

			if (i == count_pic) {
				break;
			}
		}
	}

	last_call[peer_id] = time(0);
}

void Rule34Plugin::update(std::string text, long peer_id, long from_id)
{
	
}
