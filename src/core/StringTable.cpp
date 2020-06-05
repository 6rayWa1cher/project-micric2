//
// Created by 6rayWa1cher and Throder on 04.06.2020.
//

#include <algorithm>
#include "../include/StringTable.h"

const std::string& StringTable::operator[](const unsigned int index) const {
	return this->_strings[index];
}

std::shared_ptr<StringOperand> StringTable::add(const std::string& name) {
	auto it = std::find(this->_strings.begin(), this->_strings.end(), name);
	unsigned int index;
	if (it != this->_strings.end()) {
		index = std::distance(this->_strings.begin(), it);
	} else {
		this->_strings.push_back(name);
		index = this->_strings.size() - 1;
	}
	StringTable *st = this;
	return std::make_shared<StringOperand>(index, st);
}
