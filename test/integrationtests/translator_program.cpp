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

auto assertStEquals(SymbolTable st1, SymbolTable st2) {
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

TEST(TranslatorProgramTests, Grammar2_18_28_29_30_31) {
	SymbolTable expected = SymbolTableBuilder()
			.withFunc("func", 2, "int")
			.withVar("a", "int", 0, 0)
			.withVar("b", "char", 0, 0)
			.withVar("!temp1", "int", 0, 0)
			.withVar("first", "int", 1, -1)
			.withVar("x", "char", 65, -1)
			.withFunc("g", 1, "int")
			.withVar("f", "int", 0, 6)
			.withVar("!temp2", "int", 0, 6)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int func(int a, char b) {a = b - 2;}"
	                                                      "int first = 1;"
	                                                      "char x = 65;"
	                                                      "int g(int f) { first = func(first, x);}"));
}

TEST(TranslatorProgramTests, Grammar2_19_32) {
	SymbolTable expected = SymbolTableBuilder()
			.withVar("a", "int", 2, -1)
			.withVar("b", "int", 4, -1)
			.withFunc("func", 2, "int")
			.withVar("a", "int", 0, 2)
			.withVar("b", "int", 0, 2)
			.withVar("!temp1", "int", 0, 2)
			.withVar("!temp2", "int", 0, 2)
			.build();
	return assertStEquals(expected, getSymbolTableProgram("int a = 2; int b = 4;"
	                                                      "int func(int a, int b) {"
	                                                      "while (a < b) { a = a + 1; }"
	                                                      "return a;}"));
}

TEST(TranslatorProgramTests, Grammar2_44_45_48) {
	std::vector<std::string> expected = {
			"0\t(MUL, 1[a], `2`, 2[!temp1])",
			"0\t(NE, 2[!temp1], `0`, L1)",
			"0\t(MOV, `2`,, 1[a])",
			"0\t(JMP,,, L0)",
			"0\t(LBL,,, L1)",
			"0\t(NE, 2[!temp1], `1`, L2)",
			"0\t(MOV, `-2`,, 1[a])",
			"0\t(JMP,,, L0)",
			"0\t(LBL,,, L2)",
			"0\t(JMP,,, L0)",
			"0\t(LBL,,, L0)",
			"0\t(RET,,, `0`)"
	};
	ASSERT_EQ(expected, getAtomsProgram(
			"int main() {"
			"   int a;"
			"   switch(a * 2) {"
			"       case 0: a = 2;"
			"       case 1: a = -2;"
			"   }"
			"}"
	));
}

TEST(TranslatorProgramTests, Grammar2_47_49) {
	std::vector<std::string> expected = {
			"0\t(MUL, 1[a], `2`, 2[!temp1])",
			"0\t(NE, 2[!temp1], `0`, L1)",
			"0\t(MOV, `2`,, 1[a])",
			"0\t(JMP,,, L0)",
			"0\t(LBL,,, L1)",
			"0\t(NE, 2[!temp1], `1`, L2)",
			"0\t(MOV, `-2`,, 1[a])",
			"0\t(JMP,,, L0)",
			"0\t(LBL,,, L2)",
			"0\t(JMP,,, L3)",
			"0\t(LBL,,, L4)",
			"0\t(MOV, `-1`,, 1[a])",
			"0\t(JMP,,, L0)",
			"0\t(LBL,,, L3)",
			"0\t(JMP,,, L4)",
			"0\t(LBL,,, L0)",
			"0\t(RET,,, `0`)"
	};
	ASSERT_EQ(expected, getAtomsProgram(
			"int main() {"
			"   int a;"
			"   switch(a * 2) {"
			"       case 0: a = 2;"
			"       case 1: a = -2;"
			"       default: a = -1;"
			"   }"
			"}"
	));
}

TEST(TranslatorProgramTests, Grammar2_46) {
	ASSERT_ANY_THROW(getAtomsProgram(
			"int main() {"
			"   int a;"
			"   switch(a * 2) {"
			"       case 0: a = 2;"
			"       default: a = -2;"
			"       default: a = -1;"
			"   }"
			"}"
	));
}

TEST(TranslatorProgramTests, Grammar2_19_32_) {
	auto actual = getAtomsProgram("int a = 2; int b = 4;"
								  "int func(int a, int b) {"
	  								"while (a < b) {  "
										"a = a + 1;"
									"}"
									"return a;"
								  "}");
	std::vector<std::string> expected = {"2\t(LBL,,, L0)",
										 "2\t(MOV, `1`,, 5[!temp1])",
										 "2\t(LT, 3[a], 4[b], L2)",
										 "2\t(MOV, `0`,, 5[!temp1])",
										 "2\t(LBL,,, L2)",
										 "2\t(EQ, 5[!temp1], `0`, L1)",
										 "2\t(ADD, 3[a], `1`, 6[!temp2])",
										 "2\t(MOV, 6[!temp2],, 3[a])",
										 "2\t(JMP,,, L0)",
										 "2\t(LBL,,, L1)",
										 "2\t(RET,,, 3[a])",
										 "2\t(RET,,, `0`)"};
	ASSERT_EQ(actual, expected);
}

TEST(TranslatorProgramTests, Grammar2_20_26_33_34_36_39_51_52) {
	auto actual = getAtomsProgram("int func(int a, int b) {"
									"for (a = 1; a < 2; ++a) {"
										"for (b = 10; !(b == 15); ++b) {"
											"out a + b;"
										"} "
									"}"
									"return 0; "
								  "}");
	std::vector<std::string> expected = {"0\t(MOV, `1`,, 1[a])",
										 "0\t(LBL,,, L0)",
										 "0\t(MOV, `1`,, 3[!temp1])",
										 "0\t(LT, 1[a], `2`, L4)",
										 "0\t(MOV, `0`,, 3[!temp1])",
										 "0\t(LBL,,, L4)",
										 "0\t(EQ, 3[!temp1], `0`, L3)",
										 "0\t(JMP,,, L2)",
										 "0\t(LBL,,, L1)",
										 "0\t(ADD, 1[a], `1`, 1[a])",
										 "0\t(JMP,,, L0)",
										 "0\t(LBL,,, L2)",
										 "0\t(MOV, `10`,, 2[b])",
										 "0\t(LBL,,, L5)",
										 "0\t(MOV, `1`,, 5[!temp3])",
										 "0\t(EQ, 2[b], `15`, L9)",
										 "0\t(MOV, `0`,, 5[!temp3])",
										 "0\t(LBL,,, L9)",
										 "0\t(NOT, 5[!temp3],, 4[!temp2])",
										 "0\t(EQ, 4[!temp2], `0`, L8)",
										 "0\t(JMP,,, L7)",
										 "0\t(LBL,,, L6)",
										 "0\t(ADD, 2[b], `1`, 2[b])",
										 "0\t(JMP,,, L5)",
										 "0\t(LBL,,, L7)",
										 "0\t(ADD, 1[a], 2[b], 6[!temp4])",
										 "0\t(OUT,,, 6[!temp4])",
										 "0\t(JMP,,, L6)",
										 "0\t(LBL,,, L8)",
										 "0\t(JMP,,, L1)",
										 "0\t(LBL,,, L3)",
										 "0\t(RET,,, `0`)",
										 "0\t(RET,,, `0`)" };
	ASSERT_EQ(actual, expected);
}

TEST(TranslatorProgramTests, Grammar2_21_41_42_50_51_53) {
	auto actual = getAtomsProgram("int func(int aba, int caba) {"
									"int x;"
									"if(aba <= caba) {"
										"in x;"
									"}"
									"else {"
										"out \"abacaba\";"
									"}"
									"return 1;"
								  "}");
	std::vector<std::string> expected = { "0\t(MOV, `1`,, 4[!temp1])",
										  "0\t(LE, 1[aba], 2[caba], L2)",
										  "0\t(MOV, `0`,, 4[!temp1])",
										  "0\t(LBL,,, L2)",
										  "0\t(EQ, 4[!temp1], `0`, L0)",
										  "0\t(IN,,, 3[x])",
										  "0\t(JMP,,, L1)",
										  "0\t(LBL,,, L0)",
										  "0\t(OUT,,, 0{abacaba})",
										  "0\t(LBL,,, L1)",
										  "0\t(RET,,, `1`)",
										  "0\t(RET,,, `0`)"};
	ASSERT_EQ(actual, expected);
}

#pragma clang diagnostic pop