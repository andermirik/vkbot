#pragma once
#include <string>
#include "http/uri.h"

class BaseAttachment {
public:
	std::string type;
	long long id;
	long long owner_id;
	long long date;
};

class PhotoAttachment : public BaseAttachment{
	http::Uri uri;
};