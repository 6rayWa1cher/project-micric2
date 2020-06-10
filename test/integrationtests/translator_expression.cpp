//
// Created by 6rayWa1cher on 05.06.2020.
//

#include <gtest/gtest.h>
#include "../../src/include/Atoms.h"
#include "../../src/include/SymbolTable.h"
#include "../../src/include/StringTable.h"
#include "../../src/include/Translator.h"
#include "../tools.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"




TEST(TranslatorExpressionTests, Grammar1_2) {
	std::vector<std::string> expected = {
			"(OR, 0[a], `2`, 2[!temp1])",
			"(OR, 2[!temp1], 1[b], 3[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("a || 2 || b", {"a", "b"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_3_4) {
	std::vector<std::string> expected = {
			"(OR, 0[a], `2`, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("a || 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_5) {
	std::vector<std::string> expected = {
			"(AND, 0[a], `2`, 2[!temp1])",
			"(AND, 2[!temp1], 1[b], 3[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("a && 2 && b", {"a", "b"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_6_7) {
	std::vector<std::string> expected = {
			"(AND, 0[a], `2`, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("a && 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_8) {
	ASSERT_ANY_THROW(getAtomsExpression("a == 2 == r", {"a", "r"}));
}

TEST(TranslatorExpressionTests, Grammar1_9) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(EQ, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a == 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_10) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(NE, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a != 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_11) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(GT, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a > 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_12) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(LT, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a < 2", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_13) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 1[!temp1])",
			"(LE, 0[a], `2`, L0)",
			"(MOV, `0`,, 1[!temp1])",
			"(LBL,,, L0)"
	};
	std::vector<std::string> actual = getAtomsExpression("a <= 2", {"a"});
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorExpressionTests, Grammar1_14_15_16_17_18) {
	std::vector<std::string> expected = {
			"(ADD, `2`, `3`, 0[!temp1])",
			"(SUB, 0[!temp1], `4`, 1[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("2 + 3 - 4", {});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_19_21) {
	std::vector<std::string> expected = {
			"(MUL, `2`, `101`, 0[!temp1])",
			"(MUL, 0[!temp1], `4`, 1[!temp2])"
	};
	std::vector<std::string> actual = getAtomsExpression("2 * 'e' * 4", {});
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorExpressionTests, Grammar1_20_23) {
	std::vector<std::string> expected = {
			"(MUL, `2`, `3`, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("2 * 3", {});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_22) {
	std::vector<std::string> expected = {
			"(NOT, `6`,, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("!6", {});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_24) {
	std::vector<std::string> expected = {
			"(ADD, 1[b], 2[c], 4[!temp2])",
			"(MUL, 0[a], 4[!temp2], 3[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("a * (b + c)", {"a", "b", "c"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_25) {
	getAtomsExpression("2", {});
}

TEST(TranslatorExpressionTests, Grammar1_26) {
	std::vector<std::string> expected = {
			"(NOT, `101`,, 0[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("!'e'", {});
	ASSERT_EQ(expected, actual);
}


TEST(TranslatorExpressionTests, Grammar1_27) {
	std::vector<std::string> expected = {
			"(ADD, 0[a], `1`, 0[a])"
	};
	std::vector<std::string> actual = getAtomsExpression("++a", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_28) {
	std::vector<std::string> expected = {
			"(NOT, 0[a],, 1[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("!a", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_29) {
	std::vector<std::string> expected = {
			"(MOV, 0[a],, 1[!temp1])",
			"(ADD, 0[a], `1`, 0[a])"
	};
	std::vector<std::string> actual = getAtomsExpression("a++", {"a"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, Grammar1_31) {
	std::vector<std::string> expected = {
			"(MUL, 0[a], 1[b], 2[!temp1])"
	};
	std::vector<std::string> actual = getAtomsExpression("a * b", {"a", "b"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, BigTest1) {
	std::vector<std::string> expected = {
			"(MOV, `1`,, 4[!temp1])",
			"(EQ, 0[a], 1[b], L0)",
			"(MOV, `0`,, 4[!temp1])",
			"(LBL,,, L0)",
			"(MUL, 4[!temp1], `3`, 5[!temp2])",

			"(SUB, `1`, `65`, 7[!temp4])",
			"(MUL, 2[c], `2`, 9[!temp6])",
			"(ADD, 7[!temp4], 9[!temp6], 8[!temp5])",
			"(MOV, 3[d],, 11[!temp8])",
			"(ADD, 3[d], `1`, 3[d])",
			"(SUB, 8[!temp5], 11[!temp8], 10[!temp7])",

			"(MOV, `1`,, 6[!temp3])",
			"(GT, 5[!temp2], 10[!temp7], L1)",
			"(MOV, `0`,, 6[!temp3])",
			"(LBL,,, L1)"
	};
	std::vector<std::string> actual = getAtomsExpression("(a == b) * 3 > 1 - 'A' + c * 2 - d++", {"a", "b", "c", "d"});
	ASSERT_EQ(expected, actual);
}

TEST(TranslatorExpressionTests, BigTest2) {
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