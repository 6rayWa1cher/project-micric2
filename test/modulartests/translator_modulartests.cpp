//
// Created by 6rayWa1cher and Throder-TVRS on 04.06.2020.
//

#include <gtest/gtest.h>
#include "../../src/include/Atoms.h"
#include "../../src/include/SymbolTable.h"
#include "../../src/include/StringTable.h"
#include "../../src/include/Translator.h"
#include "../../src/include/GlobalParameters.h"
#include "../tools.h"
#include <sstream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST(StringTableTests, Overall) {
	GlobalParameters::getInstance().enableOperatorFormatter = false;
	StringTable table;
	auto string = "Hello world!";
	std::shared_ptr<StringOperand> ptr = table.add(string);
	ASSERT_FALSE(!ptr);
	ASSERT_EQ(std::string("S0"), ptr->toString());
	ASSERT_EQ(string, table[0]);

	ASSERT_EQ(std::string("S0"), table.add(string)->toString());

	auto string2 = "Hello world again!";
	std::shared_ptr<StringOperand> ptr2 = table.add(string2);
	ASSERT_FALSE(!ptr2);
	ASSERT_EQ(std::string("S1"), ptr2->toString());
	ASSERT_EQ(string2, table[1]);

	ASSERT_EQ(std::string("S1"), table.add(string2)->toString());
}

TEST(StringTableTests, OverallFormatter) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	StringTable table;
	auto string = "Hello world!";
	std::shared_ptr<StringOperand> ptr = table.add(string);
	ASSERT_FALSE(!ptr);
	ASSERT_EQ(std::string("S0{") + string + "}", ptr->toString());
	ASSERT_EQ(string, table[0]);

	ASSERT_EQ(std::string("S0{") + string + "}", table.add(string)->toString());

	auto string2 = "Hello world again!";
	std::shared_ptr<StringOperand> ptr2 = table.add(string2);
	ASSERT_FALSE(!ptr2);
	ASSERT_EQ(std::string("S1{") + string2 + "}", ptr2->toString());
	ASSERT_EQ(string2, table[1]);

	ASSERT_EQ(std::string("S1{") + string2 + "}", table.add(string2)->toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(SymbolTableTests, Overall) {
	GlobalParameters::getInstance().enableOperatorFormatter = false;
	SymbolTable table;
	auto string = "meaningOfLife";
	std::shared_ptr<MemoryOperand> ptr = table.addVar(string, 42, SymbolTable::TableRecord::RecordType::integer);
	ASSERT_FALSE(!ptr);
	ASSERT_EQ(std::string("0"), ptr->toString());
	ASSERT_EQ(string, table[0]);
	ASSERT_NE(nullptr, table.checkVar(42, string));
	ASSERT_EQ(MemoryOperand(0, &table), *table.checkVar(42, string));
	ASSERT_EQ(nullptr, table.addVar(string, 42, SymbolTable::TableRecord::RecordType::integer));

	auto string2 = "meaningOfLife2";
	std::shared_ptr<MemoryOperand> ptr2 = table.addFunc(string2, SymbolTable::TableRecord::RecordType::chr, 3);
	ASSERT_FALSE(!ptr2);
	ASSERT_EQ(std::string("1"), ptr2->toString());
	ASSERT_EQ(string2, table[1]);
	ASSERT_NE(nullptr, table.checkFunc(string2, 3));
	ASSERT_EQ(MemoryOperand(1, &table), *table.checkFunc(string2, 3));
	ASSERT_EQ(nullptr, table.addFunc(string2, SymbolTable::TableRecord::RecordType::chr, 3));

	std::shared_ptr<MemoryOperand> ptr3 = table.alloc(777);
	ASSERT_FALSE(!ptr3);
	ASSERT_EQ("2", ptr3->toString());
	ASSERT_EQ("!temp1", table[2]);
	ASSERT_NE(nullptr, table.checkVar(777, "!temp1"));
	ASSERT_EQ(MemoryOperand(2, &table), *table.checkVar(777, "!temp1"));
	ASSERT_EQ("3", table.alloc(777)->toString());
}

TEST(SymbolTableTests, OverallFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	auto string = "meaningOfLife";
	std::shared_ptr<MemoryOperand> ptr = table.addVar(string, 42, SymbolTable::TableRecord::RecordType::integer);
	ASSERT_FALSE(!ptr);
	ASSERT_EQ(std::string("0[") + string + "]", ptr->toString());
	ASSERT_EQ(string, table[0]);
	ASSERT_NE(nullptr, table.checkVar(42, string));
	ASSERT_EQ(MemoryOperand(0, &table), *table.checkVar(42, string));
	ASSERT_EQ(nullptr, table.addVar(string, 42, SymbolTable::TableRecord::RecordType::integer));

	auto string2 = "meaningOfLife2";
	std::shared_ptr<MemoryOperand> ptr2 = table.addFunc(string2, SymbolTable::TableRecord::RecordType::chr, 3);
	ASSERT_FALSE(!ptr2);
	ASSERT_EQ(std::string("1[") + string2 + "]", ptr2->toString());
	ASSERT_EQ(string2, table[1]);
	ASSERT_NE(nullptr, table.checkFunc(string2, 3));
	ASSERT_EQ(MemoryOperand(1, &table), *table.checkFunc(string2, 3));
	ASSERT_EQ(nullptr, table.addFunc(string2, SymbolTable::TableRecord::RecordType::chr, 3));

	std::shared_ptr<MemoryOperand> ptr3 = table.alloc(777);
	ASSERT_FALSE(!ptr3);
	ASSERT_EQ("2[!temp1]", ptr3->toString());
	ASSERT_EQ("!temp1", table[2]);
	ASSERT_NE(nullptr, table.checkVar(777, "!temp1"));
	ASSERT_EQ(MemoryOperand(2, &table), *table.checkVar(777, "!temp1"));
	ASSERT_EQ("3[!temp2]", table.alloc(777)->toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(SymbolTableTests, PrintTableTest) {
	const std::string expected =
			"SYMBOL TABLE\n"
			"------------------------------------------------------------------\n"
			"code    name      kind    type    len     init    scope   offset  \n"
			"0       a         var     integer None    0       -1      -1      \n"
			"1       b         var     chr     None    0       -1      -1      \n"
			"2       !temp1    var     integer None    0       -1      -1      \n";
	SymbolTable symbolTable;
	symbolTable.addVar("a", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	symbolTable.addVar("b", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::chr);
	symbolTable.alloc(GLOBAL_SCOPE);
	std::ostringstream ss;
	symbolTable.printSymbolTable(ss);
	ASSERT_EQ(expected, ss.str());
}

TEST(SymbolTableTests, LongNamePrintTableTest) {
	const std::string expected =
		"SYMBOL TABLE\n"
		"--------------------------------------------------------------------------------------\n"
		"code    name                          kind    type    len     init    scope   offset  \n"
		"0       a                             var     integer None    0       -1      -1      \n"
		"1       b                             var     chr     None    0       -1      -1      \n"
		"2       !temp1                        var     integer None    0       -1      -1      \n"
		"3       OhThatReallyLongFunctionName  func    integer 2       0       -1      -1      \n";
	SymbolTable symbolTable;
	symbolTable.addVar("a", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	symbolTable.addVar("b", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::chr);
	symbolTable.alloc(GLOBAL_SCOPE);
	symbolTable.addFunc("OhThatReallyLongFunctionName", SymbolTable::TableRecord::RecordType::integer, 2);
	std::ostringstream ss;
	symbolTable.printSymbolTable(ss);
	ASSERT_EQ(expected, ss.str());
}

TEST(AtomTests, MemoryOperand) {
	SymbolTable table;
	auto string = "some symbol";
	table.addVar(string, GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	MemoryOperand memoryOperand(0, &table);
	ASSERT_EQ(std::string("0"), memoryOperand.toString());

	auto string2 = "some symbol2";
	table.addVar(string2, GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	MemoryOperand memoryOperand2(1, &table);
	ASSERT_EQ(std::string("1"), memoryOperand2.toString());
}

TEST(AtomTests, MemoryOperandFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	auto string = "some symbol";
	table.addVar(string, GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	MemoryOperand memoryOperand(0, &table);
	ASSERT_EQ(std::string("0[") + string + "]", memoryOperand.toString());

	auto string2 = "some symbol2";
	table.addVar(string2, GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	MemoryOperand memoryOperand2(1, &table);
	ASSERT_EQ(std::string("1[") + string2 + "]", memoryOperand2.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, NumberOperand) {
	NumberOperand numberOperand(1337);
	ASSERT_EQ("`1337`", numberOperand.toString());
}

TEST(AtomTests, StringOperand) {
	StringTable table;
	auto string = "some string";
	table.add(string);
	StringOperand stringOperand(0, &table);
	ASSERT_EQ(std::string("S0"), stringOperand.toString());

	auto string2 = "some string2";
	table.add(string2);
	StringOperand stringOperand2(1, &table);
	ASSERT_EQ(std::string("S1"), stringOperand2.toString());
}

TEST(AtomTests, StringOperandFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	StringTable table;
	auto string = "some string";
	table.add(string);
	StringOperand stringOperand(0, &table);
	ASSERT_EQ(std::string("S0{") + string + "}", stringOperand.toString());

	auto string2 = "some string2";
	table.add(string2);
	StringOperand stringOperand2(1, &table);
	ASSERT_EQ(std::string("S1{") + string2 + "}", stringOperand2.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, LabelOperand) {
	LabelOperand labelOperand(2020);
	ASSERT_EQ("2020", labelOperand.toString());
}

TEST(AtomTests, LabelOperandFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	LabelOperand labelOperand(2020);
	ASSERT_EQ("L2020", labelOperand.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}


TEST(AtomTests, BinaryOpAtom) {
	SymbolTable cTable;
	const std::string addName = "ADD";
	std::shared_ptr<MemoryOperand> x = cTable.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<NumberOperand> n = std::make_shared<NumberOperand>(10);
	std::shared_ptr<MemoryOperand> z = cTable.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	BinaryOpAtom binaryAtomAdd(addName, x, n, z);
	ASSERT_EQ("(ADD, 0, `10`, 1)", binaryAtomAdd.toString());

	const std::string subName = "SUB";
	n = std::make_shared<NumberOperand>(250);
	BinaryOpAtom binaryAtomSub(subName, n, z, x);
	ASSERT_EQ("(SUB, `250`, 1, 0)", binaryAtomSub.toString());

	const std::string mulName = "MUL";
	BinaryOpAtom binaryAtomMul(mulName, x, x, z);
	ASSERT_EQ("(MUL, 0, 0, 1)", binaryAtomMul.toString());

	const std::string divName = "DIV";
	n = std::make_shared<NumberOperand>(2);
	BinaryOpAtom binaryAtomDiv(divName, z, n, x);
	ASSERT_EQ("(DIV, 1, `2`, 0)", binaryAtomDiv.toString());

	const std::string orName = "OR";
	BinaryOpAtom binaryAtomOr(orName, x, z, x);
	ASSERT_EQ("(OR, 0, 1, 0)", binaryAtomOr.toString());

	const std::string andName = "AND";
	BinaryOpAtom binaryAtomAnd(andName, x, z, x);
	ASSERT_EQ("(AND, 0, 1, 0)", binaryAtomAnd.toString());
}

TEST(AtomTests, BinaryOpAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable cTable;
	const std::string addName = "ADD";
	std::shared_ptr<MemoryOperand> x = cTable.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<NumberOperand> n = std::make_shared<NumberOperand>(10);
	std::shared_ptr<MemoryOperand> z = cTable.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	BinaryOpAtom binaryAtomAdd(addName, x, n, z);
	ASSERT_EQ("(ADD, 0[x], `10`, 1[z])", binaryAtomAdd.toString());

	const std::string subName = "SUB";
	n = std::make_shared<NumberOperand>(250);
	BinaryOpAtom binaryAtomSub(subName, n, z, x);
	ASSERT_EQ("(SUB, `250`, 1[z], 0[x])", binaryAtomSub.toString());

	const std::string mulName = "MUL";
	BinaryOpAtom binaryAtomMul(mulName, x, x, z);
	ASSERT_EQ("(MUL, 0[x], 0[x], 1[z])", binaryAtomMul.toString());

	const std::string divName = "DIV";
	n = std::make_shared<NumberOperand>(2);
	BinaryOpAtom binaryAtomDiv(divName, z, n, x);
	ASSERT_EQ("(DIV, 1[z], `2`, 0[x])", binaryAtomDiv.toString());

	const std::string orName = "OR";
	BinaryOpAtom binaryAtomOr(orName, x, z, x);
	ASSERT_EQ("(OR, 0[x], 1[z], 0[x])", binaryAtomOr.toString());

	const std::string andName = "AND";
	BinaryOpAtom binaryAtomAnd(andName, x, z, x);
	ASSERT_EQ("(AND, 0[x], 1[z], 0[x])", binaryAtomAnd.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, UnaryOpAtom) {
	SymbolTable cTable;
	const std::string negName = "NEG";
	std::shared_ptr<MemoryOperand> x = cTable.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<MemoryOperand> z = cTable.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	UnaryOpAtom unaryAtomNeg(negName, x, z);
	ASSERT_EQ("(NEG, 0,, 1)", unaryAtomNeg.toString());

	const std::string notName = "NOT";
	UnaryOpAtom unaryAtomNot(notName, x, x);
	ASSERT_EQ("(NOT, 0,, 0)", unaryAtomNot.toString());

	const std::string movName = "MOV";
	UnaryOpAtom unaryAtomMov(movName, x, z);
	ASSERT_EQ("(MOV, 0,, 1)", unaryAtomMov.toString());
}

TEST(AtomTests, UnaryOpAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable cTable;
	const std::string negName = "NEG";
	std::shared_ptr<MemoryOperand> x = cTable.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<MemoryOperand> z = cTable.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	UnaryOpAtom unaryAtomNeg(negName, x, z);
	ASSERT_EQ("(NEG, 0[x],, 1[z])", unaryAtomNeg.toString());

	const std::string notName = "NOT";
	UnaryOpAtom unaryAtomNot(notName, x, x);
	ASSERT_EQ("(NOT, 0[x],, 0[x])", unaryAtomNot.toString());

	const std::string movName = "MOV";
	UnaryOpAtom unaryAtomMov(movName, x, z);
	ASSERT_EQ("(MOV, 0[x],, 1[z])", unaryAtomMov.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, ConditionalJumpAtom) {
	SymbolTable cTable;
	const std::string eqName = "EQ";
	std::shared_ptr<MemoryOperand> x = cTable.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<MemoryOperand> z = cTable.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(10);
	ConditionalJumpAtom conditionalJumpAtomEq(eqName, x, z, l);
	ASSERT_EQ("(EQ, 0, 1, 10)", conditionalJumpAtomEq.toString());

	const std::string neName = "NE";
	ConditionalJumpAtom conditionalJumpAtomNe(neName, x, z, l);
	ASSERT_EQ("(NE, 0, 1, 10)", conditionalJumpAtomNe.toString());

	const std::string gtName = "GT";
	ConditionalJumpAtom conditionalJumpAtomMov(gtName, z, x, l);
	ASSERT_EQ("(GT, 1, 0, 10)", conditionalJumpAtomMov.toString());
}

TEST(AtomTests, ConditionalJumpAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable cTable;
	const std::string eqName = "EQ";
	std::shared_ptr<MemoryOperand> x = cTable.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<MemoryOperand> z = cTable.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(10);
	ConditionalJumpAtom conditionalJumpAtomEq(eqName, x, z, l);
	ASSERT_EQ("(EQ, 0[x], 1[z], L10)", conditionalJumpAtomEq.toString());

	const std::string neName = "NE";
	ConditionalJumpAtom conditionalJumpAtomNe(neName, x, z, l);
	ASSERT_EQ("(NE, 0[x], 1[z], L10)", conditionalJumpAtomNe.toString());

	const std::string gtName = "GT";
	ConditionalJumpAtom conditionalJumpAtomMov(gtName, z, x, l);
	ASSERT_EQ("(GT, 1[z], 0[x], L10)", conditionalJumpAtomMov.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, JumpAtom) {
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(10);
	JumpAtom jumpAtom(l);
	ASSERT_EQ("(JMP,,, 10)", jumpAtom.toString());
}

TEST(AtomTests, JumpAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(10);
	JumpAtom jumpAtom(l);
	ASSERT_EQ("(JMP,,, L10)", jumpAtom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;

}

TEST(AtomTests, LabelAtom) {
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(3);
	LabelAtom labelAtom(l);
	ASSERT_EQ("(LBL,,, 3)", labelAtom.toString());
}

TEST(AtomTests, LabelAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(3);
	LabelAtom labelAtom(l);
	ASSERT_EQ("(LBL,,, L3)", labelAtom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, InAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	InAtom inAtom(x);
	ASSERT_EQ("(IN,,, 0)", inAtom.toString());
}

TEST(AtomTests, InAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	InAtom inAtom(x);
	ASSERT_EQ("(IN,,, 0[x])", inAtom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, OutAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	OutAtom outAtom(x);
	ASSERT_EQ("(OUT,,, 0)", outAtom.toString());
}

TEST(AtomTests, OutAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	OutAtom outAtom(x);
	ASSERT_EQ("(OUT,,, 0[x])", outAtom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, CallAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> f = table.addFunc("func1", SymbolTable::TableRecord::RecordType::integer, 1);
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	CallAtom callAtom(f, x);
	ASSERT_EQ("(CALL, 0,, 1)", callAtom.toString());
}

TEST(AtomTests, CallAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	std::shared_ptr<MemoryOperand> f = table.addFunc("func1", SymbolTable::TableRecord::RecordType::integer, 1);
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	CallAtom callAtom(f, x);
	ASSERT_EQ("(CALL, 0[func1],, 1[x])", callAtom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, RetAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> v = table.addVar("g", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	RetAtom retAtom(v);
	ASSERT_EQ("(RET,,, 0)", retAtom.toString());
}

TEST(AtomTests, RetAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	std::shared_ptr<MemoryOperand> v = table.addVar("g", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	RetAtom retAtom(v);
	ASSERT_EQ("(RET,,, 0[g])", retAtom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(AtomTests, ParamAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> v = table.addVar("y", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	ParamAtom paramAtom(v);
	ASSERT_EQ("(PARAM,,, 0)", paramAtom.toString());
}

TEST(AtomTests, ParamAtomFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	std::shared_ptr<MemoryOperand> v = table.addVar("y", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	ParamAtom paramAtom(v);
	ASSERT_EQ("(PARAM,,, 0[y])", paramAtom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(TranslatorTests, AddNPrint) {
	auto iss = std::istringstream("1 + 2");
	Translator translator(iss);
	std::ostringstream ss;
	translator.printAtoms(ss);
	ASSERT_EQ("", ss.str());

	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<MemoryOperand> z = table.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(10);
	std::shared_ptr<ConditionalJumpAtom> conditionalJumpAtomEq = std::make_shared<ConditionalJumpAtom>("EQ", x, z, l);

	translator.generateAtoms(GLOBAL_SCOPE, conditionalJumpAtomEq);

	x = table.addVar("u", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	z = table.addVar("v", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<BinaryOpAtom> binaryAtomAdd = std::make_shared<BinaryOpAtom>("ADD", x, z, z);

	translator.generateAtoms(GLOBAL_SCOPE, binaryAtomAdd);

	ss.clear();

	translator.printAtoms(ss);
	auto expected = "-1\t(EQ, 0, 1, 10)\n"
	                "-1\t(ADD, 2, 3, 3)\n";
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorTests, AddNPrintFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	auto iss = std::istringstream("1 + 2");
	Translator translator(iss);
	std::ostringstream ss;
	translator.printAtoms(ss);
	ASSERT_EQ("", ss.str());

	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<MemoryOperand> z = table.addVar("z", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(10);
	std::shared_ptr<ConditionalJumpAtom> conditionalJumpAtomEq = std::make_shared<ConditionalJumpAtom>("EQ", x, z, l);

	translator.generateAtoms(GLOBAL_SCOPE, conditionalJumpAtomEq);

	x = table.addVar("u", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	z = table.addVar("v", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	std::shared_ptr<BinaryOpAtom> binaryAtomAdd = std::make_shared<BinaryOpAtom>("ADD", x, z, z);

	translator.generateAtoms(GLOBAL_SCOPE, binaryAtomAdd);

	ss.clear();

	translator.printAtoms(ss);
	auto expected = "-1\t(EQ, 0[x], 1[z], L10)\n"
		"-1\t(ADD, 2[u], 3[v], 3[v])\n";
	ASSERT_EQ(expected, ss.str());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(SymbolTableTests, allocTest) {
	SymbolTable table;
	auto temp = table.alloc(GLOBAL_SCOPE);
	UnaryOpAtom atom("NEG", temp, temp);
	ASSERT_EQ("(NEG, 0,, 0)", atom.toString());
}

TEST(SymbolTableTests, allocTestFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	SymbolTable table;
	auto temp = table.alloc(GLOBAL_SCOPE);
	UnaryOpAtom atom("NEG", temp, temp);
	ASSERT_EQ("(NEG, 0[!temp1],, 0[!temp1])", atom.toString());
	GlobalParameters::getInstance().enableOperatorFormatter = false;

}

TEST(TranslatorTests, newLabelTest) {
	auto iss = std::istringstream("1 + 2");
	Translator translator(iss);
	std::ostringstream ss;

	std::shared_ptr<LabelOperand> l = translator.newLabel();
	std::shared_ptr<LabelAtom> labelAtom = std::make_shared<LabelAtom>(l);
	translator.generateAtoms(GLOBAL_SCOPE, labelAtom);

	l = translator.newLabel();
	labelAtom = std::make_shared<LabelAtom>(l);
	translator.generateAtoms(GLOBAL_SCOPE, labelAtom);

	translator.printAtoms(ss);
	auto expected = "-1\t(LBL,,, 0)\n"
	                "-1\t(LBL,,, 1)\n";
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorTests, newLabelTestFormatted) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	auto iss = std::istringstream("1 + 2");
	Translator translator(iss);
	std::ostringstream ss;

	std::shared_ptr<LabelOperand> l = translator.newLabel();
	std::shared_ptr<LabelAtom> labelAtom = std::make_shared<LabelAtom>(l);
	translator.generateAtoms(GLOBAL_SCOPE, labelAtom);

	l = translator.newLabel();
	labelAtom = std::make_shared<LabelAtom>(l);
	translator.generateAtoms(GLOBAL_SCOPE, labelAtom);

	translator.printAtoms(ss);
	auto expected = "-1\t(LBL,,, L0)\n"
		"-1\t(LBL,,, L1)\n";
	ASSERT_EQ(expected, ss.str());
	GlobalParameters::getInstance().enableOperatorFormatter = false;
}

TEST(TranslatorExceptionsTests, lexicalUnexpectedEOFTest) {
	std::ostringstream ss;
	std::string expected = "LEXICAL ERROR: [error, \"unexpected EOF\"]\n"
	                       "Appeared on the 1 line.\n"
	                       "Last correctly read lexemes: [lpar], [id, \"a\"], [rpar]\n";
	try {
		getAtomsExpression("(a) &", {"a"});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, lexicalUnexpectedANDTest) {
	std::ostringstream ss;
	std::string expected = "LEXICAL ERROR: [error, \"unexpected statement: expected &&\"]\n"
	                       "Appeared on the 1 line.\n"
	                       "Last correctly read lexemes: [lpar], [id, \"a\"], [rpar]\n";
	try {
		getAtomsExpression("(a) & ", {"a"});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, lexicalUnexpectedORTest) {
	std::ostringstream ss;
	std::string expected = "LEXICAL ERROR: [error, \"unexpected statement: expected ||\"]\n"
	                       "Appeared on the 1 line.\n"
	                       "Last correctly read lexemes: [lpar], [id, \"a\"], [rpar]\n";
	try {
		getAtomsExpression("(a) | ", {"a"});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, lexicalUnknownSymbolTest) {
	std::ostringstream ss;
	std::string expected = "LEXICAL ERROR: [error, \"unknown symbol \\\"]\n"
	                       "Appeared on the 1 line.\n"
	                       "Last correctly read lexemes: [nothing], [nothing], [nothing]\n";
	try {
		getAtomsExpression("\\", {});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, syntaxReachedEOFBeforeEndTest) {
	std::ostringstream ss;
	std::string expected = "SYNTAX ERROR: Reached EOF before the end of syntax analysis\n"
	                       "Appeared on the 1 line.\n"
	                       "Current lexem: [eof]\n"
	                       "Last correctly read lexemes: [nothing], [nothing], [nothing]\n";
	try {
		getAtomsExpression("", {});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, syntaxAdditionalLexemeAppearedTest) {
	std::ostringstream ss;
	std::string expected = "SYNTAX ERROR: Syntax analysis was completed, but an additional lexeme appeared\n"
	                       "Appeared on the 1 line.\n"
	                       "Current lexem: [id, \"b\"]\n"
	                       "Last correctly read lexemes: [nothing], [id, \"a\"], [opinc]\n";
	try {
		getAtomsExpression("a++ b", {"a", "b"});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, syntaxExpectedRparTest) {
	std::ostringstream ss;
	std::string expected = "SYNTAX ERROR: Not matching lexeme type: expected - [rpar], got - [id, \"b\"]\n"
	                       "Appeared on the 1 line.\n"
	                       "Current lexem: [id, \"b\"]\n"
	                       "Last correctly read lexemes: [lpar], [id, \"a\"], [opinc]\n";
	try {
		getAtomsExpression("(a++ b)", {"a", "b"});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, syntaxExpectedNumChrTest) {
	std::ostringstream ss;
	std::string expected = "SYNTAX ERROR: Not matching lexeme type: expected - [lpar], [num], [chr], [opinc] or [id], got - [rpar]\n"
	                       "Appeared on the 1 line.\n"
	                       "Current lexem: [rpar]\n"
	                       "Last correctly read lexemes: [nothing], [nothing], [lpar]\n";
	try {
		getAtomsExpression("()", {});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorExceptionsTests, syntaxExpectedIdTest) {
	std::ostringstream ss;
	std::string expected = "SYNTAX ERROR: Not matching lexeme type: expected - [id], got - [lpar]\n"
	                       "Appeared on the 1 line.\n"
	                       "Current lexem: [lpar]\n"
	                       "Last correctly read lexemes: [id, \"a\"], [opplus], [opinc]\n";
	try {
		getAtomsExpression("a + ++()", {"a"});
	}
	catch (TranslationException& exception) {
		ss << exception.what();
	}
	ASSERT_EQ(expected, ss.str());
}

TEST(SymbolTableTests, loadTest) {
    SymbolTable table;
    SymbolTable::TableRecord record;
    record._name = "x";
    record._init = 14;
    record._kind = SymbolTable::TableRecord::RecordKind::var;
    record._type = SymbolTable::TableRecord::RecordType::integer;
    record._len = -1;
    record._offset = -1;
    record._scope = -1;
    table._records.push_back(record);
    record._name = "y";
    record._init = 64;
	record._kind = SymbolTable::TableRecord::RecordKind::var;
	record._type = SymbolTable::TableRecord::RecordType::chr;
	record._len = -1;
	record._offset = 0;
	record._scope = 2;
	table._records.push_back(record);
	auto x = std::make_shared<MemoryOperand>(0, &table);
	auto y = std::make_shared<MemoryOperand>(1, &table);
	auto z = std::make_shared<NumberOperand>(25565);
	std::ostringstream ss;
	x->load(ss, 0);
	ASSERT_EQ(ss.str(), "LDA var0\n");
	std::ostringstream ss1;
	y->load(ss1, 0);
	ASSERT_EQ(ss1.str(), "LXI H, 0\nDAD SP\nMOV A, M\n");
	std::ostringstream ss2;
	z->load(ss2, 0);
	ASSERT_EQ(ss2.str(), "MVI A, 25565\n");
}

TEST(SymbolTableTests, saveTest) {
    SymbolTable table;
    SymbolTable::TableRecord record;
    record._name = "x";
    record._init = 14;
    record._kind = SymbolTable::TableRecord::RecordKind::var;
    record._type = SymbolTable::TableRecord::RecordType::integer;
    record._len = -1;
    record._offset = -1;
    record._scope = -1;
    table._records.push_back(record);
    record._name = "y";
    record._init = 64;
	record._kind = SymbolTable::TableRecord::RecordKind::var;
	record._type = SymbolTable::TableRecord::RecordType::chr;
	record._len = -1;
	record._offset = 0;
	record._scope = 2;
	table._records.push_back(record);
	auto x = std::make_shared<MemoryOperand>(0, &table);
	auto y = std::make_shared<MemoryOperand>(1, &table);
	std::ostringstream ss;
	x->save(ss);
	ASSERT_EQ(ss.str(), "STA var0\n");
	std::ostringstream ss1;
	y->save(ss1);
	ASSERT_EQ(ss1.str(), "LXI H, 0\nDAD SP\nMOV M, A\n");
}

TEST(SymbolTableTests, generateGlobalsTest) {
    SymbolTable table;
    SymbolTable::TableRecord record;
    record._name = "x";
    record._init = 14;
    record._kind = SymbolTable::TableRecord::RecordKind::var;
    record._type = SymbolTable::TableRecord::RecordType::integer;
    record._len = -1;
    record._offset = -1;
    record._scope = -1;
    table._records.push_back(record);
    record._name = "y";
    record._init = 64;
    record._kind = SymbolTable::TableRecord::RecordKind::var;
    record._type = SymbolTable::TableRecord::RecordType::chr;
    record._len = -1;
    record._offset = 0;
    record._scope = -1;
    table._records.push_back(record);
    std::ostringstream ss;
    table.generateGlobals(ss);
    ASSERT_EQ(ss.str(), "var0: DB 14\nvar1: DB 64\n");
}

TEST(SymbolTableTests, generateStringsTest) {
    StringTable table;
    table.add("Abacaba");
    table.add("ABACABA");
    std::ostringstream ss;
    table.generateStrings(ss);
    ASSERT_EQ(ss.str(), "str0: DB \'Abacaba\', 0\nstr1: DB \'ABACABA\', 0\n");
}

#pragma clang diagnostic pop