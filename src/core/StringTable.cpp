//
// Created by 6rayWa1cher on 04.06.2020.
//

#include "../include/StringTable.h"

const std::string& StringTable::operator[](const unsigned int index) const {
	return this->_strings[index];
}

std::shared_ptr<StringOperand> StringTable::add(const std::string& name) {
	this->_strings.push_back(name);
	unsigned int index = this->_strings.size() - 1;
	StringTable *st = this;
	return std::make_shared<StringOperand>(index, st);
}
