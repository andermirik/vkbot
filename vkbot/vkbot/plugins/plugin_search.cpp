#include "plugin_search.h"
#include <iomanip>
#include "../html_parser/Document.h"
#include "../html_parser/Node.h"

SearchPlugin::SearchPlugin(const std::vector<std::vector<std::string>>& command, CallArea call_area) : Plugin(command, call_area) {
}

void SearchPlugin::exec(const std::vector<std::string>& args, long peer_id, long from_id) {
	bool have_attachment = false;
	int pos_attachment = -1;

	bool have_photo = false;
	json pic;

	for (int i = 0; i < args.size();i++) {
		if (args[i] == "attachments") {
			have_attachment = true;
			pos_attachment = i + 1;
			break;
		}
	}

	if (have_attachment) {
		pic = json::parse(args[pos_attachment])[0];
		if (pic["type"].get<std::string>() == "photo")
			have_photo = true;
	}

	if (have_photo) {
		pic = pic["photo"];

		std::string url;
		int max = 0;
		for (auto& size : pic["sizes"]) {
			if (size["width"] > max) {
				max = size["width"];
				url = size["url"].get<std::string>();
			}
		}

		http::Request re;
		re.method = "GET";
		re.headers["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:68.0) Gecko/20100101 Firefox/68.0";
		re.uri = http::Uri("https://yandex.ru/images/search?url=" + url + "&rpt=imageview");

		auto result = http::sendRequest(re);
		while (result.status_code == 302) {
			re.uri = http::Uri(result.headers["Location"]);
			result = http::sendRequest(re);
		}

		std::stringstream ret;
		if (result.status_code == 200) {
			CDocument doc;
			doc.parse(result.body);
			CSelection c = doc.find(".tags__wrapper a");
			ret << "ƒумаю, на изображении что-то из этого: \r\n";
			for (int i = 0; i < c.nodeNum(); i++)
				ret << "Х " << to_ascii(c.nodeAt(i).text()) << "\r\n";
		}

		vk::apisay(urlencode(to_utf8(ret.str())), std::to_string(peer_id));
	}
	else {
		vk::apisay(urlencode(to_utf8("прикрепи картинку")), std::to_string(peer_id));
	}
}

void SearchPlugin::update(std::string text, long peer_id, long from_id)
{
}
