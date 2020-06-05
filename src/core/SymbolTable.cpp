//
// Created by 6rayWa1cher on 04.06.2020.
//

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
	this->_records.push_back(tableRecord);
	unsigned int index = this->_records.size() - 1;
	SymbolTable *st = this;
	return std::make_shared<MemoryOperand>(index, st);
}

const std::string& SymbolTable::operator[](const unsigned int index) const {
	return this->_records[index]._name;
}

std::shared_ptr<MemoryOperand> SymbolTable::alloc() {
	return add("");
}
