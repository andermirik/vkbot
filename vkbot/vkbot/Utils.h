#pragma once
#include <string>
#include <vector>
#include "windows.h"
#include <sstream>
#include <iostream> 

std::string join(std::vector<std::string> v, std::string separator);
std::string char2hex(const char dec);
std::vector<std::string> split(const std::string& s, char seperator);
std::string urlencode(const std::string &url);
std::string to_utf8(const std::string& ascii_str);
std::string to_ascii(std::string utf8);
std::string to_lower(std::string str);