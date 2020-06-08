//
// Created by 6rayWa1cher on 08.06.2020.
//

#include "tools.h"
#include <sstream>

std::vector<std::string> split(const std::string& string, char delimiter) {
	std::vector<std::string> out;
	std::stringstream sb;
	for (char c : string) {
		if (c == delimiter) {
			out.push_back(sb.str());
			sb = std::stringstream();
		} else {
			sb << c;
		}
	}
	out.push_back(sb.str());
	return out;
}