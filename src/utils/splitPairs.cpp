#include "webserv/utils.hpp"

std::pair<std::string, std::string> splitPair(std::string str, char c) {
	std::pair<std::string, std::string> kv;
	size_t								k = str.find(c);
	if (k != std::string::npos && k != 0) {
		kv.first = str.substr(0, k);
		kv.second = str.substr(k + 1);
	}
	return kv;
}
