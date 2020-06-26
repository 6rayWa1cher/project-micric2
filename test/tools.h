//
// Created by 6rayWa1cher on 08.06.2020.
//

#ifndef PROJECT_MICRIC2_TOOLS_H
#define PROJECT_MICRIC2_TOOLS_H

#include "../src/include/Translator.h"
#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string& string, char delimiter);

std::vector<std::string> getAtomsExpression(const std::string& s, std::vector<std::string> vars, Scope scope = 1337);


class SymbolTableBuilder {
private:
	std::shared_ptr<SymbolTable> st;

	explicit SymbolTableBuilder(std::shared_ptr<SymbolTable> symbolTable,
	                            std::vector<std::shared_ptr<MemoryOperand>> operands);

	static SymbolTable::TableRecord::RecordType toRecordType(const std::string& recordType);

public:
	SymbolTableBuilder();

	std::vector<std::shared_ptr<MemoryOperand>> operands;

	SymbolTableBuilder
	withVar(const std::string& name, const std::string& recordType, int init = 0, Scope scope = GLOBAL_SCOPE);

	SymbolTableBuilder withFunc(const std::string& name, int len = 0, const std::string& recordType = "int");

	SymbolTable build();

	std::pair<std::shared_ptr<SymbolTable>, std::vector<std::shared_ptr<MemoryOperand>>> buildPair();
};

#endif //PROJECT_MICRIC2_TOOLS_H
