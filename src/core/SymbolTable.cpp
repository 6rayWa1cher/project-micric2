//
// Created by 6rayWa1cher and Throder-TVRS on 04.06.2020.
//

#include <algorithm>
#include <vector>
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

const std::string& SymbolTable::operator[](const unsigned int index) const {
	return this->_records.at(index)._name;
}

std::shared_ptr<MemoryOperand> SymbolTable::alloc(Scope scope) {
	return addVar("!temp" + std::to_string(++(this->lastTemp)), scope, TableRecord::RecordType::integer);
}

size_t SymbolTable::size() const {
	return this->_records.size();
}

size_t SymbolTable::getM(Scope scope) const {
    if (scope < 0 || scope >= _records.size()) return 0;
    int64_t variableAmount = 0;
    for (const auto & _record : _records) {
        if (_record._scope == scope && _record._kind == TableRecord::RecordKind::var) variableAmount++;
    }
    return size_t(variableAmount - _records[scope]._len);
}

void SymbolTable::calculateOffset() {
    int n, m, i;
    TableRecord rec;
    for (auto& _record : _records) {
        rec = _record;
        if (rec._kind == TableRecord::RecordKind::func) continue;
        n = _records[rec._scope]._len;
        m = getM(rec._scope);
        i = 1;
        for (auto& record : _records) {
            if (record._kind == TableRecord::RecordKind::var && record._scope == rec._scope) {
                if (i <= n) record._offset = 2 * (m + n + 1 - i);
                else record._offset = 2 * (m + n - i);
                i++;
            }
        }
    }
}

std::vector<std::pair<std::string, int>> SymbolTable::functionNames() const {
    std::vector<std::pair<std::string, int>> functions;
    for (int i = 0; i < _records.size(); i++) {
        if (_records[i]._kind == TableRecord::RecordKind::func) {
            functions.emplace_back(_records[i]._name, i);
        }
    }
    return functions;
}

void printNSymbols(std::ostream& stream, size_t n, const std::string& s) {
	for (size_t i = 0; i < n; i++) stream << s;
}

void SymbolTable::printSymbolTable(std::ostream& stream) const {
	size_t nameSpace = 8;
	for (const auto& record : _records) {
		nameSpace = std::max(nameSpace, record._name.size());
	}
	stream << "SYMBOL TABLE" << std::endl;
	printNSymbols(stream, 56 + nameSpace + 2, "-");
	stream << std::endl;
	stream << "code";
	printNSymbols(stream, 4, " ");
	stream << "name";
	printNSymbols(stream, nameSpace - 2, " ");
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
		printNSymbols(stream, nameSpace - _records[i]._name.size() + 2, " ");
		if (_records[i]._kind == TableRecord::RecordKind::unknown) {
			stream << "unknown ";
		} else if (_records[i]._kind == TableRecord::RecordKind::var) {
			stream << "var";
			printNSymbols(stream, 5, " ");
		} else if (_records[i]._kind == TableRecord::RecordKind::func) {
			stream << "func";
			printNSymbols(stream, 4, " ");
		}
		if (_records[i]._type == TableRecord::RecordType::unknown) {
			stream << "unknown ";
		} else if (_records[i]._type == TableRecord::RecordType::integer) {
			stream << "integer ";
		} else if (_records[i]._type == TableRecord::RecordType::chr) {
			stream << "chr";
			printNSymbols(stream, 5, " ");
		}
		if (_records[i]._len == -1) {
			stream << "None    ";
		}
		else {
			stream << _records[i]._len;
			printNSymbols(stream, 8 - std::to_string(_records[i]._len).size(), " ");
		}
		stream << _records[i]._init;
		printNSymbols(stream, 8 - std::to_string(_records[i]._init).size(), " ");
		stream << _records[i]._scope;
		printNSymbols(stream, 8 - std::to_string(_records[i]._scope).size(), " ");
		stream << _records[i]._offset;
		printNSymbols(stream, 8 - std::to_string(_records[i]._offset).size(), " ");
		stream << std::endl;
	}
}

int64_t SymbolTable::findRecord(const std::string& name, Scope scope) const {
	for (size_t i = 0; i < _records.size(); ++i) {
		const SymbolTable::TableRecord& record = _records[i];
		if (record._name == name && record._scope == scope) {
			return i;
		}
	}
	return -1;
}

std::shared_ptr<MemoryOperand>
SymbolTable::addVar(const std::string& name, Scope scope, SymbolTable::TableRecord::RecordType type, int init) {
	int64_t recordIndex = findRecord(name, scope);
	if (recordIndex != -1) {
		return nullptr;
	} else {
		SymbolTable::TableRecord tableRecord;
		tableRecord._name = name;
		tableRecord._scope = scope;
		tableRecord._init = init;
		tableRecord._type = type;
		tableRecord._kind = TableRecord::RecordKind::var;
		this->_records.push_back(tableRecord);
		size_t index = this->_records.size() - 1;
		SymbolTable *st = this;
		return std::make_shared<MemoryOperand>(index, st);
	}
}

std::shared_ptr<MemoryOperand>
SymbolTable::addFunc(const std::string& name, SymbolTable::TableRecord::RecordType type, int len) {
	int64_t recordIndex = findRecord(name, GLOBAL_SCOPE);
	if (recordIndex != -1) {
		return nullptr;
	} else {
		SymbolTable::TableRecord tableRecord;
		tableRecord._name = name;
		tableRecord._scope = GLOBAL_SCOPE;
		tableRecord._len = len;
		tableRecord._type = type;
		tableRecord._kind = TableRecord::RecordKind::func;
		this->_records.push_back(tableRecord);
		size_t index = this->_records.size() - 1;
		SymbolTable *st = this;
		return std::make_shared<MemoryOperand>(index, st);
	}
}

std::shared_ptr<MemoryOperand> SymbolTable::checkVar(Scope scope, const std::string& name) {
	int64_t recordIndex = findRecord(name, scope);
	if (recordIndex == -1) {
		recordIndex = findRecord(name, GLOBAL_SCOPE);
	}
	if (recordIndex == -1 || _records[recordIndex]._kind != SymbolTable::TableRecord::RecordKind::var) {
		return nullptr;
	}
	SymbolTable *st = this;
	return std::make_shared<MemoryOperand>(recordIndex, st);
}

std::shared_ptr<MemoryOperand> SymbolTable::checkFunc(const std::string& name, int len) {
	int64_t recordIndex = findRecord(name, GLOBAL_SCOPE);
	TableRecord *record;
	if (recordIndex == -1 || (record = &_records[recordIndex])->_kind != SymbolTable::TableRecord::RecordKind::func ||
	    record->_len != len) {
		return nullptr;
	}
	SymbolTable *st = this;
	return std::make_shared<MemoryOperand>(recordIndex, st);
}

bool SymbolTable::operator==(const SymbolTable& rhs) const {
	return _records == rhs._records &&
	       lastTemp == rhs.lastTemp;
}

bool SymbolTable::operator!=(const SymbolTable& rhs) const {
	return !(rhs == *this);
}

void SymbolTable::changeFuncLength(const std::string& name, int newLen) {
	auto it = std::find_if(_records.begin(), _records.end(), [name](const TableRecord& tableRecord) -> bool {
		return tableRecord._name == name && tableRecord._kind == TableRecord::RecordKind::func;
	});
	if (it != _records.end()) {
		it->_len = newLen;
	}
}

void SymbolTable::generateGlobals(std::ostream &stream) const {
    for(size_t i = 0; i < _records.size(); i++) {
        if(_records[i]._scope == -1 && _records[i]._kind != SymbolTable::TableRecord::RecordKind::func) {
            stream << "var" + std::to_string(i) + ": DB " + std::to_string(_records[i]._init) + "\n";
        }
    }
}