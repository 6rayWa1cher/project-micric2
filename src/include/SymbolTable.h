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

const Scope GLOBAL_SCOPE = -1;

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
		Scope _scope = GLOBAL_SCOPE;
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

	size_t getNextIndex() const;

	std::shared_ptr<MemoryOperand> addVar(const std::string& name,
	                                      const Scope scope, TableRecord::RecordType type,
	                                      int init = 0);

	std::shared_ptr<MemoryOperand> addFunc(const std::string& name, TableRecord::RecordType type, int len);

	void changeFuncLength(const std::string& name, int newLen);

	virtual std::shared_ptr<MemoryOperand> checkVar(const Scope scope, const std::string& name);

	std::shared_ptr<MemoryOperand> checkFunc(const std::string& name, int len);

	std::shared_ptr<MemoryOperand> alloc(Scope scope);

	void printSymbolTable(std::ostream& stream) const;

	bool operator==(const SymbolTable& rhs) const;

	bool operator!=(const SymbolTable& rhs) const;

private:
	int64_t findRecord(const std::string& name, Scope scope) const;
};

#endif //PROJECT_MICRIC2_SYMBOLTABLE_H
