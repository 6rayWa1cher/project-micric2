//
// Created by 6rayWa1cher and Throder-TVRS on 04.06.2020.
//

#include <algorithm>
#include <iostream>
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

void StringTable::printStringTable(std::ostream& stream) {
	stream << "STRING TABLE:" << std::endl;
	for (size_t i = 0; i < 64; i++) stream << "-";
	stream << std::endl;
	for (size_t i = 0; i < _strings.size(); i++) {
		stream << i << "       " << _strings[i] << std::endl;
	}
}

void StringTable::generateStrings(std::ostream &stream) const {
    for(size_t i = 0; i < _strings.size(); i++) {
        stream << "str" + std::to_string(i) + ": DB \'" + _strings[i] + "\', 0\n";
    }
}
