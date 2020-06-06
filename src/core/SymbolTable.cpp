//
// Created by 6rayWa1cher and Throder on 04.06.2020.
//

#include <algorithm>
#include "../include/SymbolTable.h"

bool SymbolTable::TableRecord::operator==(const SymbolTable::TableRecord& rhs) const {
	return (_name == rhs._name &&
		_kind == rhs._kind &&
		_type == rhs._type &&
		_len == rhs._len &&
		_init == rhs._init &&
		_scope == rhs._scope &&
		_offset == rhs._offset);
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

void printNSymbols(std::ostream& stream, size_t n, std::string s) {
	for (size_t i = 0; i < n; i++) stream << s;
}

void SymbolTable::printSymbolTable(std::ostream& stream) {
	//Designed for a column length of not more than 8
	//For function names or variables of greater length, it is necessary to rework
	stream << "SYMBOL TABLE" << std::endl;
	printNSymbols(stream, 64, "-");
	stream << std::endl;
	stream << "code";
	printNSymbols(stream, 4, " ");
	stream << "name";
	printNSymbols(stream, 4, " ");
	stream << "kind";
	printNSymbols(stream, 4, " ");
	stream << "type";
	printNSymbols(stream, 4, " ");
	stream << "len";
	printNSymbols(stream, 5, " ");
	stream << "init";
	printNSymbols(stream, 4, " ");
	stream << "scope";
	printNSymbols(stream, 3, " ");
	stream << "offset";
	printNSymbols(stream, 2, " ");
	stream << std::endl;
	for (size_t i = 0; i < _records.size(); i++) {
		stream << i;
		printNSymbols(stream, 8 - std::to_string(i).size(), " ");
		stream << _records[i]._name;
		printNSymbols(stream, 8 - _records[i]._name.size(), " ");
		if (_records[i]._kind == TableRecord::RecordKind::unknown) {
			stream << "unknown ";
		}
		else if (_records[i]._kind == TableRecord::RecordKind::var) {
			stream << "var";
			printNSymbols(stream, 5, " ");
		}
		else if (_records[i]._kind == TableRecord::RecordKind::func) {
			stream << "func";
			printNSymbols(stream, 4, " ");
		}
		if (_records[i]._type == TableRecord::RecordType::unknown) {
			stream << "unknown ";
		}
		else if (_records[i]._type == TableRecord::RecordType::integer) {
			stream << "integer ";
		}
		else if (_records[i]._type == TableRecord::RecordType::chr) {
			stream << "fun";
			printNSymbols(stream, 5, " ");
		}
		stream << _records[i]._len;
		printNSymbols(stream, 8 - std::to_string(_records[i]._len).size(), " ");
		stream << _records[i]._init;
		printNSymbols(stream, 8 - std::to_string(_records[i]._init).size(), " ");
		stream << _records[i]._scope;
		printNSymbols(stream, 8 - std::to_string(_records[i]._scope).size(), " ");
		stream << _records[i]._offset;
		printNSymbols(stream, 8 - std::to_string(_records[i]._offset).size(), " ");
		stream << std::endl;
	}
}