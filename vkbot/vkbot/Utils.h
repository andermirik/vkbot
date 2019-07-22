#pragma once
#include <string>
#include <vector>
#include "windows.h"

std::string char2hex(const char dec);
std::vector<std::string> split(const std::string& s, char seperator);
std::string urlencode(const std::string &url);
std::string to_utf8(const std::string& ascii_str);
std::string to_ascii(std::string utf8);
