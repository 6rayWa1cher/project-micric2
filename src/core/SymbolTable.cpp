//
// Created by 6rayWa1cher and Throder on 04.06.2020.
//

#include <algorithm>
#include "../include/SymbolTable.h"

bool SymbolTable::TableRecord::operator==(const SymbolTable::TableRecord& rhs) const {
	return _name == rhs._name;
}

bool SymbolTable::TableRecord::operator!=(const SymbolTable::TableRecord& rhs) const {
	return !(rhs == *this);
}

std::shared_ptr<MemoryOperand> SymbolTable::add(const std::string& name) {
	SymbolTable::TableRecord tableRecord;
	tableRecord._name = name;
	auto it = std::find_if(this->_records.begin(), this->_records.end(), [name](const SymbolTable::TableRecord& a) {
		return a._name == name;
	});
	unsigned int index;
	if (it != _records.end()) {
		index = std::distance(this->_records.begin(), it);
	} else {
		this->_records.push_back(tableRecord);
		index = this->_records.size() - 1;
	}
	SymbolTable *st = this;
	return std::make_shared<MemoryOperand>(index, st);
}

const std::string& SymbolTable::operator[](const unsigned int index) const {
	return this->_records[index]._name;
}

std::shared_ptr<MemoryOperand> SymbolTable::alloc() {
	return add("!temp" + std::to_string(++(this->lastTemp)));
}

size_t SymbolTable::size() const {
	return this->_records.size();
}
