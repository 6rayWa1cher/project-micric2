//
// Created by 6rayWa1cher on 26.06.2020.
//

#include <gtest/gtest.h>

#include <utility>
#include "../src/include/Atoms.h"
#include "../src/include/Translator.h"
#include "tools.h"
#include "../src/include/GlobalParameters.h"

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
			"LDA var12\n"
			"MOV B, A\n"
			"LDA var42\n"
			"ADD B\n"
			"STA var13\n",
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

#pragma clang diagnostic pop