//
// Created by 6rayWa1cher on 04.06.2020.
//

#include <gtest/gtest.h>
#include "../src/include/Atoms.h"
#include "../src/include/SymbolTable.h"
#include "../src/include/StringTable.h"
#include "../src/include/Translator.h"
#include <sstream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST(StringTableTests, Overall) {
	StringTable table;
	auto string = "Hello world!";
	std::shared_ptr<StringOperand> ptr = table.add(string);
	ASSERT_FALSE(!ptr);
	ASSERT_EQ(std::string("0{") + string + "}", ptr->toString());
	ASSERT_EQ(string, table[0]);

	ASSERT_EQ(std::string("0{") + string + "}", table.add(string)->toString());

	auto string2 = "Hello world again!";
	std::shared_ptr<StringOperand> ptr2 = table.add(string2);
	ASSERT_FALSE(!ptr2);
	ASSERT_EQ(std::string("1{") + string2 + "}", ptr2->toString());
	ASSERT_EQ(string2, table[1]);

	ASSERT_EQ(std::string("1{") + string2 + "}", table.add(string2)->toString());
}

TEST(SymbolTableTests, Overall) {
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
}

TEST(SymbolTableTests, PrintTableTest) {
	const std::string expected =
			"SYMBOL TABLE\n"
			"----------------------------------------------------------------\n"
			"code    name    kind    type    len     init    scope   offset  \n"
			"0       a       var     integer -1      0       -1      -1      \n"
			"1       b       var     chr     -1      0       -1      -1      \n"
			"2       !temp1  var     integer -1      0       -1      -1      \n";
	SymbolTable symbolTable;
	symbolTable.addVar("a", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	symbolTable.addVar("b", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::chr);
	symbolTable.alloc(GLOBAL_SCOPE);
	std::ostringstream ss;
	symbolTable.printSymbolTable(ss);
	ASSERT_EQ(expected, ss.str());
}

TEST(AtomTests, MemoryOperand) {
	SymbolTable table;
	auto string = "some symbol";
	table.addVar(string, GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	MemoryOperand memoryOperand(0, &table);
	ASSERT_EQ(std::string("0[") + string + "]", memoryOperand.toString());

	auto string2 = "some symbol2";
	table.addVar(string2, GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	MemoryOperand memoryOperand2(1, &table);
	ASSERT_EQ(std::string("1[") + string2 + "]", memoryOperand2.toString());
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
	ASSERT_EQ(std::string("0{") + string + "}", stringOperand.toString());

	auto string2 = "some string2";
	table.add(string2);
	StringOperand stringOperand2(1, &table);
	ASSERT_EQ(std::string("1{") + string2 + "}", stringOperand2.toString());
}

TEST(AtomTests, LabelOperand) {
	LabelOperand labelOperand(2020);
	ASSERT_EQ("L2020", labelOperand.toString());
}


TEST(AtomTests, BinaryOpAtom) {
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
}

TEST(AtomTests, UnaryOpAtom) {
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
}

TEST(AtomTests, ConditionalJumpAtom) {
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
}

TEST(AtomTests, JumpAtom) {
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(10);
	JumpAtom jumpAtom(l);
	ASSERT_EQ("(JMP,,, L10)", jumpAtom.toString());
}

TEST(AtomTests, LabelAtom) {
	std::shared_ptr<LabelOperand> l = std::make_shared<LabelOperand>(3);
	LabelAtom labelAtom(l);
	ASSERT_EQ("(LBL,,, L3)", labelAtom.toString());
}

TEST(AtomTests, InAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	InAtom inAtom(x);
	ASSERT_EQ("(IN,,, 0[x])", inAtom.toString());
}

TEST(AtomTests, OutAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	OutAtom outAtom(x);
	ASSERT_EQ("(OUT,,, 0[x])", outAtom.toString());
}

TEST(AtomTests, CallAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> f = table.addFunc("func1", SymbolTable::TableRecord::RecordType::integer, 1);
	std::shared_ptr<MemoryOperand> x = table.addVar("x", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	CallAtom callAtom(f, x);
	ASSERT_EQ("(CALL, 0[func1],, 1[x])", callAtom.toString());
}

TEST(AtomTests, RetAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> v = table.addVar("g", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	RetAtom retAtom(v);
	ASSERT_EQ("(RET,,, 0[g])", retAtom.toString());
}

TEST(AtomTests, ParamAtom) {
	SymbolTable table;
	std::shared_ptr<MemoryOperand> v = table.addVar("y", GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
	ParamAtom paramAtom(v);
	ASSERT_EQ("(PARAM,,, 0[y])", paramAtom.toString());
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
	auto expected = "-1\t(EQ, 0[x], 1[z], L10)\n"
	                "-1\t(ADD, 2[u], 3[v], 3[v])\n";
	ASSERT_EQ(expected, ss.str());
}

TEST(SymbolTableTests, allocTest) {
	SymbolTable table;
	auto temp = table.alloc(GLOBAL_SCOPE);
	UnaryOpAtom atom("NEG", temp, temp);
	ASSERT_EQ("(NEG, 0[!temp1],, 0[!temp1])", atom.toString());
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
	auto expected = "-1\t(LBL,,, L0)\n"
	                "-1\t(LBL,,, L1)\n";
	ASSERT_EQ(expected, ss.str());
}

TEST(TranslatorTests, exceptionsTest) {
	auto iss = std::istringstream("1 + 2");
	Translator translator(iss);
	std::ostringstream ss;
	try {
		translator.lexicalError("Incorrect lexem!");
	}
	catch (TranslationException& exception) {
		ss << "Translator exception accured (" << exception.what() << ")";
	}
	ASSERT_EQ("Translator exception accured (Incorrect lexem!)", ss.str());
	std::ostringstream ss1;

	try {
		translator.syntaxError("AAAA, GORIM!");
	}
	catch (TranslationException& exception1) {
		ss1 << "Translator exception accured (" << exception1.what() << ")";
	}
	ASSERT_EQ("Translator exception accured (AAAA, GORIM!)", ss1.str());
}


#pragma clang diagnostic pop