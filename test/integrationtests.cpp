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


std::vector<std::string> getAtoms(const std::string& s) {
	auto iss = std::istringstream(s);
	Translator translator(iss);
	translator.startTranslation();
	std::ostringstream oss;
	translator.printAtoms(oss);
	std::string out = oss.str();
	std::vector<std::string> outVector = split(out, '\n');
	outVector.erase(outVector.end() - 1);
	return outVector;
}

TEST(TranslatorTests, Grammar2) {
	std::vector<std::string> expected = {
			"(OR, 0[a], `2`, 1[!temp1])",
			"(OR, 1[!temp1], 2[b], 3[!temp2])"
	};
	std::vector<std::string> actual = getAtoms("a || 2 || b");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar3_4) {
	std::vector<std::string> expected = {
			"(OR, 0[a], `2`, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtoms("a || 2");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar5) {
	std::vector<std::string> expected = {
			"(AND, 0[a], `2`, 1[!temp1])",
			"(AND, 1[!temp1], 2[b], 3[!temp2])"
	};
	std::vector<std::string> actual = getAtoms("a && 2 && b");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar6_7) {
	std::vector<std::string> expected = {
			"(AND, 0[a], `2`, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtoms("a && 2");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar8) {
	ASSERT_ANY_THROW(getAtoms("a == 2 == r"));
}

TEST(TranslatorTests, Grammar9) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(EQ, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtoms("a == 2");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar10) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(NE, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtoms("a != 2");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar11) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(GT, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtoms("a > 2");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar12) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(LT, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtoms("a < 2");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar13) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(LE, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtoms("a <= 2");
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, Grammar14_15_16_17_18) {
	std::vector<std::string> expected = {
			"(ADD, `2`, `3`, 0[!temp1])",
			"(SUB, 0[!temp1], `4`, 1[!temp2])"
	};
	std::vector<std::string> actual = getAtoms("2 + 3 - 4");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar19_21) {
	std::vector<std::string> expected = {
			"(MUL, `2`, `101`, 0[!temp1])",
			"(MUL, 0[!temp1], `4`, 1[!temp2])"
	};
	std::vector<std::string> actual = getAtoms("2 * 'e' * 4");
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, Grammar20_23) {
	std::vector<std::string> expected = {
			"(MUL, `2`, `3`, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtoms("2 * 3");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar22) {
	std::vector<std::string> expected = {
			"(NOT, `6`,, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtoms("!6");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar24) {
	std::vector<std::string> expected = {
			"(ADD, 1[b], 2[c], 3[!temp1])",
			"(MUL, 0[a], 3[!temp1], 4[!temp2])"
	};
	std::vector<std::string> actual = getAtoms("a * (b + c)");
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
	std::vector<std::string> actual = getAtoms("!'e'");
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, Grammar27) {
	std::vector<std::string> expected = {
			"(ADD, 0[a], `1`, 0[a])"
	};
	std::vector<std::string> actual = getAtoms("++a");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar28) {
	std::vector<std::string> expected = {
			"(NOT, 0[a],, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtoms("!a");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar29) {
	std::vector<std::string> expected = {
			"(MOV, 0[a],, 1[!temp1])",
			"(ADD, 0[a], `1`, 0[a])"
	};
	std::vector<std::string> actual = getAtoms("a++");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, Grammar31) {
	std::vector<std::string> expected = {
			"(MUL, 0[a], 1[b], 2[!temp1])"
	};
	std::vector<std::string> actual = getAtoms("a * b");
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorTests, BigTest1) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 2[!temp1])",
			"(EQ, 0[a], 1[b], L0)",
			"(MOV, `0`,, 2[!temp1])",
			"(LBL,,, L0)",
			"(MUL, 2[!temp1], `3`, 3[!temp2])",

			"(SUB, `1`, `65`, 4[!temp3])",
			"(MUL, 5[c], `2`, 6[!temp4])",
			"(ADD, 4[!temp3], 6[!temp4], 7[!temp5])",
			"(MOV, 8[d],, 9[!temp6])",
			"(ADD, 8[d], `1`, 8[d])",
			"(SUB, 7[!temp5], 9[!temp6], 10[!temp7])",

			"(MOV, `1`,, 11[!temp8])",
			"(GT, 3[!temp2], 10[!temp7], L1)",
			"(MOV, `0`,, 11[!temp8])",
			"(LBL,,, L1)"
	};
	std::vector<std::string> actual = getAtoms("(a == b) * 3 > 1 - 'A' + c * 2 - d++");
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorTests, BigTest2) {
	ASSERT_ANY_THROW(getAtoms("(a == b) * 3 > 1 - 'A' + c * - d++"));
	ASSERT_ANY_THROW(getAtoms("(a == ) * 3 > 1 - 'A' + c * 2 - d++"));
	ASSERT_ANY_THROW(getAtoms("(a == b) * 3 > "));
	ASSERT_ANY_THROW(getAtoms("(a == b) * 3 > 1 - 'A' + c * 2 2 - d++"));
	ASSERT_ANY_THROW(getAtoms("(a == b) * 3 > 1 - 'A' + c * 2 - ++"));
	ASSERT_ANY_THROW(getAtoms("(a == b * 3 > 1 - 'A' + c * 2 - d++"));
	ASSERT_ANY_THROW(getAtoms("a == b) * 3 > 1 - 'A' + c * 2 - d++"));
	ASSERT_ANY_THROW(getAtoms("(a == b) * 3 > 1 - + c * 2 - d++"));
}

#pragma clang diagnostic pop