//
// Created by 6rayWa1cher and Throder on 04.06.2020.
//

#ifndef PROJECT_MICRIC2_SYMBOLTABLE_H
#define PROJECT_MICRIC2_SYMBOLTABLE_H
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Atoms.h"

typedef int Scope;

const Scope GlobalScope = -1;

class SymbolTable {
public:
	struct TableRecord {
		enum class RecordKind {
			unknown, var, func
		};

		enum class RecordType {
			unknown, integer, chr
		};

		std::string _name;
		RecordKind _kind = RecordKind::unknown;
		RecordType _type = RecordType::unknown;
		int _len = -1;
		int _init = 0;
		Scope _scope = GlobalScope;
		int _offset = -1;


		bool operator==(const TableRecord& rhs) const;

		bool operator!=(const TableRecord& rhs) const;
	};

private:
	std::vector<TableRecord> _records;
	int lastTemp = 0;

public:
	const std::string& operator[](const unsigned int index) const;

	size_t size() const;

	std::shared_ptr<MemoryOperand> add(const std::string& name);

	std::shared_ptr<MemoryOperand> alloc();

	void printSymbolTable(std::ostream& stream);
};

#endif //PROJECT_MICRIC2_SYMBOLTABLE_H
