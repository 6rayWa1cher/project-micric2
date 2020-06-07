//
// Created by 6rayWa1cher on 05.06.2020.
//

#include <gtest/gtest.h>
#include "../src/include/Atoms.h"
#include "../src/include/SymbolTable.h"
#include "../src/include/StringTable.h"
#include "../src/include/Translator.h"
#include <sstream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"


std::vector<std::string> split(const std::string& string, const char delimiter) {
	std::vector<std::string> out;
	std::stringstream sb;
	for (char c : string) {
		if (c == delimiter) {
			out.push_back(sb.str());
			sb = std::stringstream();
		} else {
			sb << c;
		}
	}
	out.push_back(sb.str());
	return out;
}


std::vector<std::string> getAtomsExpression(const std::string& s, std::vector<std::string> vars) {
	class LocalTranslator : public Translator {
	private:
		Scope _scope;
	public:
		explicit LocalTranslator(std::vector<std::string>& vars, Scope scope, std::istream& inputStream) : Translator(
				inputStream), _scope(scope) {
			for (const std::string& var : vars) {
				_symbolTable.addVar(var, scope, SymbolTable::TableRecord::RecordType::integer);
			}
		};

		void startTranslation() override {
			E(_scope);
			const Token& token = _scanner.getNextToken();
			if (token != LexemType::eof) {
				syntaxError("Syntax analysis ended, but additional token appears");
			}
		}
	};
	auto iss = std::istringstream(s);
	Scope expectedScope = 1337;
	LocalTranslator translator(vars, expectedScope, iss);
	translator.startTranslation();
	std::ostringstream oss;
	translator.printAtoms(oss);
	std::string out = oss.str();
	std::vector<std::string> scopeAtomVector = split(out, '\n');
	scopeAtomVector.erase(scopeAtomVector.end() - 1);
	std::vector<std::string> outVector;
	for (const std::string& scopeAtom : scopeAtomVector) {
		auto spliited = split(scopeAtom, '\t');
		Scope actualScope = std::stoi(spliited[0]);
		if (actualScope != expectedScope) {
			ADD_FAILURE();
			throw std::exception();
		}
		outVector.push_back(spliited[1]);
	}
	return outVector;
}

TEST(TranslatorTests, Grammar2) {
	std::vector<std::string> expected = {
			"(OR, 0[a], `2`, 2[!temp1])",
			"(OR, 2[!temp1], 1[b], 3[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("a || 2 || b", {"a", "b"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar3_4) {
	std::vector<std::string> expected = {
			"(OR, 0[a], `2`, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("a || 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar5) {
	std::vector<std::string> expected = {
			"(AND, 0[a], `2`, 2[!temp1])",
			"(AND, 2[!temp1], 1[b], 3[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("a && 2 && b", {"a", "b"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar6_7) {
	std::vector<std::string> expected = {
			"(AND, 0[a], `2`, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("a && 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar8) {
	ASSERT_ANY_THROW(getAtomsExpression("a == 2 == r", {"a", "r"}));
}

TEST(TranslatorTests, Grammar9) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(EQ, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a == 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar10) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(NE, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a != 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar11) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(GT, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a > 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar12) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(LT, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a < 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar13) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(LE, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a <= 2", {"a"});
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, Grammar14_15_16_17_18) {
	std::vector<std::string> expected = {
			"(ADD, `2`, `3`, 0[!temp1])",
			"(SUB, 0[!temp1], `4`, 1[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("2 + 3 - 4", {});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar19_21) {
	std::vector<std::string> expected = {
			"(MUL, `2`, `101`, 0[!temp1])",
			"(MUL, 0[!temp1], `4`, 1[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("2 * 'e' * 4", {});
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, Grammar20_23) {
	std::vector<std::string> expected = {
			"(MUL, `2`, `3`, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("2 * 3", {});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar22) {
	std::vector<std::string> expected = {
			"(NOT, `6`,, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("!6", {});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar24) {
	std::vector<std::string> expected = {
			"(ADD, 1[b], 2[c], 3[!temp1])",
			"(MUL, 0[a], 3[!temp1], 4[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("a * (b + c)", {"a", "b", "c"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar25) {
	auto iss = std::istringstream("2");
	Translator translator(iss);
	translator.startTranslation();
}

TEST(TranslatorTests, Grammar26) {
	std::vector<std::string> expected = {
			"(NOT, `101`,, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("!'e'", {});
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, Grammar27) {
	std::vector<std::string> expected = {
			"(ADD, 0[a], `1`, 0[a])"
	};
	std::vector<std::string> actual = getAtomsExpression("++a", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar28) {
	std::vector<std::string> expected = {
			"(NOT, 0[a],, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("!a", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar29) {
	std::vector<std::string> expected = {
			"(MOV, 0[a],, 1[!temp1])",
			"(ADD, 0[a], `1`, 0[a])"
	};
	std::vector<std::string> actual = getAtomsExpression("a++", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar31) {
	std::vector<std::string> expected = {
			"(MUL, 0[a], 1[b], 2[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("a * b", {"a", "b"});
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, BigTest1) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 4[!temp1])",
			"(EQ, 0[a], 1[b], L0)",
			"(MOV, `0`,, 4[!temp1])",
			"(LBL,,, L0)",
			"(MUL, 4[!temp1], `3`, 5[!temp2])",

			"(SUB, `1`, `65`, 6[!temp3])",
			"(MUL, 2[c], `2`, 7[!temp4])",
			"(ADD, 6[!temp3], 7[!temp4], 8[!temp5])",
			"(MOV, 3[d],, 9[!temp6])",
			"(ADD, 3[d], `1`, 3[d])",
			"(SUB, 8[!temp5], 9[!temp6], 10[!temp7])",

			"(MOV, `1`,, 11[!temp8])",
			"(GT, 5[!temp2], 10[!temp7], L1)",
			"(MOV, `0`,, 11[!temp8])",
			"(LBL,,, L1)"
	};
	std::vector<std::string> actual = getAtomsExpression("(a == b) * 3 > 1 - 'A' + c * 2 - d++", {"a", "b", "c", "d"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, BigTest2) {
	ASSERT_ANY_THROW(getAtomsExpression("(a == b) * 3 > 1 - 'A' + c * - d++", {"a", "b", "c", "d"}));
	ASSERT_ANY_THROW(getAtomsExpression("(a == ) * 3 > 1 - 'A' + c * 2 - d++", {"a", "b", "c", "d"}));
	ASSERT_ANY_THROW(getAtomsExpression("(a == b) * 3 > ", {"a", "b", "c", "d"}));
	ASSERT_ANY_THROW(getAtomsExpression("(a == b) * 3 > 1 - 'A' + c * 2 2 - d++", {"a", "b", "c", "d"}));
	ASSERT_ANY_THROW(getAtomsExpression("(a == b) * 3 > 1 - 'A' + c * 2 - ++", {"a", "b", "c", "d"}));
	ASSERT_ANY_THROW(getAtomsExpression("(a == b * 3 > 1 - 'A' + c * 2 - d++", {"a", "b", "c", "d"}));
	ASSERT_ANY_THROW(getAtomsExpression("a == b) * 3 > 1 - 'A' + c * 2 - d++", {"a", "b", "c", "d"}));
	ASSERT_ANY_THROW(getAtomsExpression("(a == b) * 3 > 1 - + c * 2 - d++", {"a", "b", "c", "d"}));
}

#pragma clang diagnostic pop