//
// Created by 6rayWa1cher on 26.06.2020.
//

#include <gtest/gtest.h>

#include <utility>
#include "../../src/include/Atoms.h"
#include "../../src/include/Translator.h"
#include "../tools.h"
#include "../../src/include/GlobalParameters.h"

struct Variable {
	std::string name;
	int init = 0;
	Scope scope = GLOBAL_SCOPE;

	Variable(std::string name, int init) : name(std::move(name)), init(init) {}

	Variable(std::string name, int init, Scope scope) : name(std::move(name)), init(init), scope(scope) {}
};

class LocalTranslator : public Translator {
public:
	explicit LocalTranslator(std::istream& istream) :
			Translator(istream) {

	}

	std::vector<std::shared_ptr<MemoryOperand>> operands;

	LocalTranslator(std::istream& istream, const std::vector<Variable>& variables) :
			Translator(istream) {
		for (const Variable& var : variables) {
			operands.push_back(
					_symbolTable.addVar(var.name, var.scope, SymbolTable::TableRecord::RecordType::integer, var.init)
			);
		}
	}

	SymbolTable& symbolTable() {
		return _symbolTable;
	}

	std::shared_ptr<StringOperand> addString(const std::string& s) {
		return _stringTable.add(s);
	}

	LocalTranslator(std::istream& istream, const SymbolTable& st) :
			Translator(istream) {
		_symbolTable = st;
	}

	LocalTranslator(std::istream& istream,
	                const std::pair<std::shared_ptr<SymbolTable>, std::vector<std::shared_ptr<MemoryOperand>>>& pair) :
			Translator(istream) {
		_symbolTable = *pair.first;
		operands = pair.second;
	}

	std::shared_ptr<MemoryOperand> operator[](size_t index) {
		return operands[index];
	}
};

std::string printAtom(std::shared_ptr<Atom>& atom, Translator& translator, Scope scope = GLOBAL_SCOPE) {
	GlobalParameters::getInstance().enableOperatorFormatter = false;
	std::ostringstream oss;
	atom->generate(oss, &translator, scope);
	return oss.str();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST(CodeGenTests, AddAtomNumberOperand) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<BinaryOpAtom>(
			"ADD", translator[0], std::make_shared<NumberOperand>(4), translator[1]
	);
	ASSERT_EQ(
			"\t; (ADD, 0, `4`, 1)\n"
			"MVI A, 4\n"
			"MOV B, A\n"
			"LDA var0\n"
			"ADD B\n"
			"STA var1\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, AddAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12), Variable("c", 13)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<BinaryOpAtom>(
			"ADD", translator[0], translator[1], translator[2]
	);
	ASSERT_EQ(
			"\t; (ADD, 0, 1, 2)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"ADD B\n"
			"STA var2\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, AddAtomLocal) {
	std::istringstream iss;
	auto p = SymbolTableBuilder()
			.withFunc("f", 0)
			.withVar("a", "int", 42, 0)
			.withVar("b", "int", 12, 0)
			.withVar("c", "int", 13, 0)
			.buildPair();
	LocalTranslator translator = LocalTranslator(iss, p);
	std::shared_ptr<Atom> addAtom = std::make_shared<BinaryOpAtom>(
			"ADD", translator[1], translator[2], translator[3]
	);
	ASSERT_EQ(
			"\t; (ADD, 1, 2, 3)\n"
			"LXI H, 2\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV B, A\n"
			"LXI H, 4\n"
			"DAD SP\n"
			"MOV A, M\n"
			"ADD B\n"
			"LXI H, 0\n"
			"DAD SP\n"
			"MOV M, A\n",
			printAtom(addAtom, translator, 0)
	);
}

TEST(CodeGenTests, SubAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12), Variable("c", 13)
			}
	);
	std::shared_ptr<Atom> subAtom = std::make_shared<BinaryOpAtom>(
			"SUB", translator[0], translator[1], translator[2]
	);
	ASSERT_EQ(
			"\t; (SUB, 0, 1, 2)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"SUB B\n"
			"STA var2\n",
			printAtom(subAtom, translator)
	);
}

TEST(CodeGenTests, MulAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12), Variable("c", 13)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<BinaryOpAtom>(
			"MUL", translator[0], translator[1], translator[2]
	);
	ASSERT_EQ(
			"\t; (MUL, 0, 1, 2)\n"
			"LDA var1\n"
			"MOV D, A\n"
			"LDA var0\n"
			"CALL @MULT\n"
			"MOV A, C\n"
			"STA var2\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, DivAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12), Variable("c", 13)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<BinaryOpAtom>(
			"DIV", translator[0], translator[1], translator[2]
	);
	ASSERT_EQ(
			"\t; (DIV, 0, 1, 2)\n"
			"LDA var1\n"
			"MOV D, A\n"
			"LDA var0\n"
			"CALL @DIV\n"
			"MOV A, C\n"
			"STA var2\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, NegAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<UnaryOpAtom>(
			"NEG", translator[0], translator[1]
	);
	ASSERT_EQ(
			"\t; (NEG, 0,, 1)\n"
			"LDA var0\n"
			"CMA\n"
			"INR A\n"
			"STA var1\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, AndAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12), Variable("c", 6)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<BinaryOpAtom>(
			"AND", translator[0], translator[1], translator[2]
	);
	ASSERT_EQ(
			"\t; (AND, 0, 1, 2)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"ANA B\n"
			"STA var2\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, OrAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12), Variable("c", 6)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<BinaryOpAtom>(
			"OR", translator[0], translator[1], translator[2]
	);
	ASSERT_EQ(
			"\t; (OR, 0, 1, 2)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"ORA B\n"
			"STA var2\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, NotAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<UnaryOpAtom>(
			"NOT", translator[0], translator[1]
	);
	ASSERT_EQ(
			"\t; (NOT, 0,, 1)\n"
			"LDA var0\n"
			"CMA\n"
			"STA var1\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, MovAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<UnaryOpAtom>(
			"MOV", translator[0], translator[1]
	);
	ASSERT_EQ(
			"\t; (MOV, 0,, 1)\n"
			"LDA var0\n"
			"STA var1\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, EqAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<ConditionalJumpAtom>(
			"EQ", translator[0], translator[1], translator.newLabel()
	);
	ASSERT_EQ(
			"\t; (EQ, 0, 1, 0)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"CMP B\n"
			"JZ LBL0\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, NeAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<ConditionalJumpAtom>(
			"NE", translator[0], translator[1], translator.newLabel()
	);
	ASSERT_EQ(
			"\t; (NE, 0, 1, 0)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"CMP B\n"
			"JNZ LBL0\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, GtAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<ConditionalJumpAtom>(
			"GT", translator[0], translator[1], translator.newLabel()
	);
	ASSERT_EQ(
			"\t; (GT, 0, 1, 0)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"CMP B\n"
			"JM LBL0A\n"
			"JNZ LBL0\n"
			"LBL0A:\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, LtAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<ConditionalJumpAtom>(
			"LT", translator[0], translator[1], translator.newLabel()
	);
	ASSERT_EQ(
			"\t; (LT, 0, 1, 0)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"CMP B\n"
			"JM LBL0\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, GeAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<ConditionalJumpAtom>(
			"GE", translator[0], translator[1], translator.newLabel()
	);
	ASSERT_EQ(
			"\t; (GE, 0, 1, 0)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"CMP B\n"
			"JM LBL0A\n"
			"JMP LBL0\n"
			"LBL0A:\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, LeAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(
			iss,
			{
					Variable("a", 42), Variable("b", 12)
			}
	);
	std::shared_ptr<Atom> addAtom = std::make_shared<ConditionalJumpAtom>(
			"LE", translator[0], translator[1], translator.newLabel()
	);
	ASSERT_EQ(
			"\t; (LE, 0, 1, 0)\n"
			"LDA var1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"CMP B\n"
			"JM LBL0\n"
			"JZ LBL0\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, JmpAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(iss);
	std::shared_ptr<Atom> addAtom = std::make_shared<JumpAtom>(translator.newLabel());
	ASSERT_EQ(
			"\t; (JMP,,, 0)\n"
			"JMP LBL0\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, InAtomGlobal) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(iss, {Variable("a", 42)});
	std::shared_ptr<Atom> addAtom = std::make_shared<InAtom>(translator[0]);
	ASSERT_EQ(
			"\t; (IN,,, 0)\n"
			"IN 0\n"
			"STA var0\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, OutAtomGlobalVariable) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(iss, {Variable("a", 42)});
	std::shared_ptr<Atom> addAtom = std::make_shared<OutAtom>(translator[0]);
	ASSERT_EQ(
			"\t; (OUT,,, 0)\n"
			"LDA var0\n"
			"OUT 1\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, OutAtomGlobalString) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(iss);
	std::shared_ptr<Atom> addAtom = std::make_shared<OutAtom>(translator.addString("Lorem ipsum dolor sit amet"));
	ASSERT_EQ(
			"\t; (OUT,,, S0)\n"
			"LXI H, str0\n"
			"CALL @PRINT\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, LblAtom) {
	std::istringstream iss;
	LocalTranslator translator = LocalTranslator(iss);
	std::shared_ptr<Atom> addAtom = std::make_shared<LabelAtom>(translator.newLabel());
	ASSERT_EQ(
			"\t; (LBL,,, 0)\n"
			"LBL0:\n",
			printAtom(addAtom, translator)
	);
}

TEST(CodeGenTests, NoParamArgCallAtomLocal) {
	std::istringstream iss;
	auto p = SymbolTableBuilder()
			.withFunc("f", 0)
			.withFunc("main", 0)
			.withVar("a", "int", 31, 1)
			.buildPair();
	LocalTranslator translator = LocalTranslator(iss, p);
	std::shared_ptr<Atom> addAtom = std::make_shared<CallAtom>(translator[0], translator[2]);
	ASSERT_EQ(
			"\t; (CALL, 0,, 2)\n"
			"PUSH B\n"
			"PUSH D\n"
			"PUSH H\n"
			"PUSH PSW\n"
			"LXI B, 0\n"
			"PUSH B\n"
			"CALL f\n"
			"POP B\n"
			"MOV A, B\n"
			"LXI H, 10\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP PSW\n"
			"POP H\n"
			"POP D\n"
			"POP B\n",
			printAtom(addAtom, translator, 1)
	);
}

TEST(CodeGenTests, NoParamArgCallAtomGlobal) {
	std::istringstream iss;
	auto p = SymbolTableBuilder()
			.withFunc("f", 0)
			.withFunc("main", 0)
			.withVar("a", "int", 31)
			.buildPair();
	LocalTranslator translator = LocalTranslator(iss, p);
	std::shared_ptr<Atom> addAtom = std::make_shared<CallAtom>(translator[0], translator[2]);
	ASSERT_EQ(
			"\t; (CALL, 0,, 2)\n"
			"PUSH B\n"
			"PUSH D\n"
			"PUSH H\n"
			"PUSH PSW\n"
			"LXI B, 0\n"
			"PUSH B\n"
			"CALL f\n"
			"POP B\n"
			"MOV A, B\n"
			"STA var2\n"
			"POP PSW\n"
			"POP H\n"
			"POP D\n"
			"POP B\n",
			printAtom(addAtom, translator, 1)
	);
}

TEST(CodeGenTests, ParamCallAtom) {
	std::istringstream iss;
	auto p = SymbolTableBuilder()
			.withFunc("f", 2)
			.withVar("x", "int", 0, 0)
			.withVar("y", "int", 0, 0)
			.withFunc("main", 0)
			.withVar("a", "int", 31, 3)
			.withVar("b", "int", 1, 3)
			.withVar("c", "int", 0, 3)
			.buildPair();
	LocalTranslator translator = LocalTranslator(iss, p);
	std::shared_ptr<Atom> paramAtom = std::make_shared<ParamAtom>(translator[4]);
	printAtom(paramAtom, translator, 2);
	std::shared_ptr<Atom> paramAtom1 = std::make_shared<ParamAtom>(translator[5]);
	printAtom(paramAtom1, translator, 2);
	ASSERT_EQ(2, translator.codeGenFuncArgs.size());
	std::shared_ptr<Atom> addAtom = std::make_shared<CallAtom>(translator[0], translator[6]);
	ASSERT_EQ(
			"\t; (CALL, 0,, 6)\n"
			"PUSH B\n"
			"PUSH D\n"
			"PUSH H\n"
			"PUSH PSW\n"
			"LXI B, 0\n"
			"PUSH B\n"
			"LXI B, 0\n"
			"LXI H, 16\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV C, A\n"
			"PUSH B\n"
			"LXI B, 0\n"
			"LXI H, 16\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV C, A\n"
			"PUSH B\n"
			"CALL f\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"MOV A, B\n"
			"LXI H, 10\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP PSW\n"
			"POP H\n"
			"POP D\n"
			"POP B\n",
			printAtom(addAtom, translator, 2)
	);
}

TEST(CodeGenTests, ReturnAtom) {
	std::istringstream iss;
	auto p = SymbolTableBuilder()
			.withFunc("f", 1)
			.withVar("x", "int", 0, 0)
			.withVar("t", "int", 0, 0)
			.withVar("t1", "int", 0, 0)
			.buildPair();
	LocalTranslator translator = LocalTranslator(iss, p);
	std::shared_ptr<Atom> retAtom = std::make_shared<RetAtom>(std::make_shared<NumberOperand>(42));
	ASSERT_EQ(
			"\t; (RET,,, `42`)\n"
			"MVI A, 42\n"
			"LXI H, 8\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP B\n"
			"POP B\n"
			"RET\n",
			printAtom(retAtom, translator, 0)
	);
}

#pragma clang diagnostic pop