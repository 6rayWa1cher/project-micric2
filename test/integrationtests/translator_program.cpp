//
// Created by 6rayWa1cher on 08.06.2020.
//

#include <gtest/gtest.h>

#include <utility>
#include "../../src/include/Atoms.h"
#include "../../src/include/SymbolTable.h"
#include "../../src/include/StringTable.h"
#include "../../src/include/Translator.h"
#include "../tools.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

class SymbolTableBuilder {
private:
	std::shared_ptr<SymbolTable> st;

	explicit SymbolTableBuilder(std::shared_ptr<SymbolTable> symbolTable) : st(std::move(symbolTable)) {
	}

	SymbolTable::TableRecord::RecordType toRecordType(const std::string& recordType) {
		if (recordType == "int")
			return SymbolTable::TableRecord::RecordType::integer;
		else if (recordType == "char")
			return SymbolTable::TableRecord::RecordType::chr;
		else
			return SymbolTable::TableRecord::RecordType::unknown;
	}

public:
	SymbolTableBuilder() {
		st = std::make_shared<SymbolTable>();
	};

	SymbolTableBuilder
	withVar(const std::string& name, const std::string& recordType, int init = 0, const Scope scope = GLOBAL_SCOPE) {
		st->addVar(name, scope, toRecordType(recordType), init);

		return SymbolTableBuilder(st);
	}

	SymbolTableBuilder withFunc(const std::string& name, int len = 0, const std::string& recordType = "int") {
		st->addFunc(name, toRecordType(recordType), len);
		return SymbolTableBuilder(st);
	}

	SymbolTable build() {
		return *st;
	}
};

std::vector<std::string> getAtomsProgram(const std::string& s) {
	auto iss = std::istringstream(s);
	Translator translator(iss);
	translator.startTranslation();
	std::ostringstream oss;
	translator.printAtoms(oss);
	std::string out = oss.str();
	std::vector<std::string> scopeAtomVector = split(out, '\n');
	scopeAtomVector.erase(scopeAtomVector.end() - 1);
	return scopeAtomVector;
}

SymbolTable getSymbolTableProgram(const std::string& s) {
	auto iss = std::istringstream(s);
	Translator translator(iss);
	translator.startTranslation();
	return translator.getSymbolTable();
}

auto assertStEquals(const SymbolTable& st1, const SymbolTable& st2) {
	std::stringstream ss1, ss2;
	st1.printSymbolTable(ss1);
	st2.printSymbolTable(ss2);
	ASSERT_EQ(ss1.str(), ss2.str());
	return SUCCEED();
}

TEST(TranslatorProgramTests, Grammar2_4_6_8) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "int")
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int a;"));
}

TEST(TranslatorProgramTests, Grammar2_5) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "char")
			.build();
	return assertStEquals(expected, getSymbolTableProgram("char a;"));
}

TEST(TranslatorProgramTests, Grammar2_3) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "int", 42)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int a = 42;"));
}

TEST(TranslatorProgramTests, Grammar2_7_8_9_10$1) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "int", 42)
			.withVar("b", "int", 12)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int a = 42, b = 12;"));
}

TEST(TranslatorProgramTests, Grammar2_7_8_9_10$2) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "int", 42)
			.withVar("b", "int")
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int a = 42, b;"));
}

TEST(TranslatorProgramTests, Grammar2_7_8_9_10$3) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "int")
			.withVar("b", "int", 42)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int a, b = 42;"));
}

TEST(TranslatorProgramTests, Grammar2_7_8_9_10$4) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "int")
			.withVar("b", "int")
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int a, b;"));
}

TEST(TranslatorProgramTests, Grammar2_2_12) {
	SymbolTable expected = SymbolTableBuilder()
			.withFunc("f")
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int f(){}"));
}

TEST(TranslatorProgramTests, Grammar2_11_14) {
	SymbolTable expected = SymbolTableBuilder()
			.withFunc("f", 1)
			.withVar("a", "char", 0, 0)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int f(char a){}"));
}

TEST(TranslatorProgramTests, Grammar2_13) {
	SymbolTable expected = SymbolTableBuilder()
			.withFunc("f", 2, "char")
			.withVar("a", "int", 0, 0)
			.withVar("b", "char", 0, 0)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("char f(int a, char b){}"));
}

TEST(TranslatorProgramTests, Grammar2_1) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("first", "int", 1)
			.withFunc("f", 2)
			.withVar("x", "int", 0, 1)
			.withVar("y", "int", 0, 1)
			.withVar("a", "int", 2, 1)
			.withFunc("g", 1, "char")
			.withVar("x", "int", 0, 5)
			.withVar("a", "int", 7, 5)
			.withVar("a", "int", 2)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int first = 1;"
	                                                      "int f(int x, int y){int a = 2;}"
	                                                      "char g(int x){int a = 7;}"
	                                                      "int a = 2;"));
}

#pragma clang diagnostic pop