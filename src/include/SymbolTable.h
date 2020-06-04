//
// Created by 6rayWa1cher on 04.06.2020.
//

#ifndef PROJECT_MICRIC2_SYMBOLTABLE_H
#define PROJECT_MICRIC2_SYMBOLTABLE_H


#include <string>
#include <vector>
#include <memory>
#include "Atoms.h"

class SymbolTable {
public:
	struct TableRecord {
		std::string _name;

		bool operator==(const TableRecord& rhs) const;

		bool operator!=(const TableRecord& rhs) const;
	};

private:
	std::vector<TableRecord> _records;
public:
	const std::string& operator[](const unsigned int index) const;

	std::shared_ptr<MemoryOperand> add(const std::string& name);
};

#endif //PROJECT_MICRIC2_SYMBOLTABLE_H
