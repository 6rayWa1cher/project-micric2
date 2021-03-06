//
// Created by 6rayWa1cher on 26.06.2020.
//


#include <gtest/gtest.h>

#include <sstream>
#include <utility>
#include "../../src/include/Atoms.h"
#include "../../src/include/Translator.h"
#include "../tools.h"
#include "../../src/include/GlobalParameters.h"

TEST(CodeGenTests, Integration1) {
	GlobalParameters::getInstance().enableOperatorFormatter = false;
	GlobalParameters::getInstance().printAsmHeader = true;
	std::istringstream iss(
			"int g = 9;"
			"int abc(int a, int b) {"
			"   char c;"
			"   if (a + 4 > b && a <= b) {"
			"       out \"anomaly\";"
			"       a = a + 2;"
			"       return a;"
			"   } else {"
			"       g = g - 1;"
			"       return 91 * g;"
			"   }"
			"}"
			"int main() {"
			"   int a, b;"
			"   in a;"
			"   in b;"
			"   out abc(abc(a, b), !abc(a, b));"
			"}"
	);
	Translator translator(iss);
	std::ostringstream oss;
	translator.startTranslation();
	translator.generateCode(oss);
	ASSERT_EQ(
			"ASM 8080 code:\n"
			"----------------------------------------------------------------\n"
			"ORG 8000H\n"
			"var0: DB 9\n"
			"str0: DB 'anomaly', 0\n"
			"ORG 0\n"
			"LXI H, 0\n"
			"SPHL\n"
			"CALL main\n"
			"END\n"
			"@MULT:\n"
			"; Code for MULT library function\n"
			"@PRINT:\n"
			"; Code for PRINT library function\n"
			"\n"
			"abc:\n"
			"LXI B, 0\n" // 22 - RETVAL, 20 - a, 18 - b
			"PUSH B\n" // 14, c
			"PUSH B\n" // 12, !temp1 (5 )
			"PUSH B\n" // 10, !temp2 (6 )
			"PUSH B\n" // 8 , !temp3 (7 )
			"PUSH B\n" // 6 , !temp4 (8 )
			"PUSH B\n" // 4 , !temp5 (9 )
			"PUSH B\n" // 2 , !temp6 (10)
			"PUSH B\n" // 0 , !temp7 (11)
			"\t; (ADD, 2, `4`, 5)\n"
			"MVI A, 4\n"
			"MOV B, A\n"
			"LXI H, 20\n"
			"DAD SP\n"
			"MOV A, M\n"
			"ADD B\n"
			"LXI H, 12\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (MOV, `1`,, 6)\n"
			"MVI A, 1\n"
			"LXI H, 10\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (GT, 5, 3, 2)\n"
			"LXI H, 18\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV B, A\n"
			"LXI H, 12\n"
			"DAD SP\n"
			"MOV A, M\n"
			"CMP B\n"
			"JM LBL2A\n"
			"JNZ LBL2\n"
			"LBL2A:\n"
			"\t; (MOV, `0`,, 6)\n"
			"MVI A, 0\n"
			"LXI H, 10\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (LBL,,, 2)\n"
			"LBL2:\n"
			"\t; (MOV, `1`,, 8)\n"
			"MVI A, 1\n"
			"LXI H, 6\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (LE, 2, 3, 3)\n"
			"LXI H, 18\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV B, A\n"
			"LXI H, 20\n"
			"DAD SP\n"
			"MOV A, M\n"
			"CMP B\n"
			"JM LBL3\n"
			"JZ LBL3\n"
			"\t; (MOV, `0`,, 8)\n"
			"MVI A, 0\n"
			"LXI H, 6\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (LBL,,, 3)\n"
			"LBL3:\n"
			"\t; (AND, 6, 8, 7)\n"
			"LXI H, 6\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV B, A\n"
			"LXI H, 10\n"
			"DAD SP\n"
			"MOV A, M\n"
			"ANA B\n"
			"LXI H, 8\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (EQ, 7, `0`, 0)\n"
			"MVI A, 0\n"
			"MOV B, A\n"
			"LXI H, 8\n"
			"DAD SP\n"
			"MOV A, M\n"
			"CMP B\n"
			"JZ LBL0\n"
			"\t; (OUT,,, S0)\n"
			"LXI H, str0\n"
			"CALL @PRINT\n"
			"\t; (ADD, 2, `2`, 9)\n"
			"MVI A, 2\n"
			"MOV B, A\n"
			"LXI H, 20\n"
			"DAD SP\n"
			"MOV A, M\n"
			"ADD B\n"
			"LXI H, 4\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (MOV, 9,, 2)\n"
			"LXI H, 4\n"
			"DAD SP\n"
			"MOV A, M\n"
			"LXI H, 20\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (RET,,, 2)\n"
			"LXI H, 20\n"
			"DAD SP\n"
			"MOV A, M\n"
			"LXI H, 22\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"RET\n"
			"\t; (JMP,,, 1)\n"
			"JMP LBL1\n"
			"\t; (LBL,,, 0)\n"
			"LBL0:\n"
			"\t; (SUB, 0, `1`, 10)\n"
			"MVI A, 1\n"
			"MOV B, A\n"
			"LDA var0\n"
			"SUB B\n"
			"LXI H, 2\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (MOV, 10,, 0)\n"
			"LXI H, 2\n"
			"DAD SP\n"
			"MOV A, M\n"
			"STA var0\n"
			"\t; (MUL, `91`, 0, 11)\n"
			"LDA var0\n"
			"MOV D, A\n"
			"MVI A, 91\n"
			"CALL @MULT\n"
			"MOV A, C\n"
			"LXI H, 0\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (RET,,, 11)\n"
			"LXI H, 0\n"
			"DAD SP\n"
			"MOV A, M\n"
			"LXI H, 22\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"RET\n"
			"\t; (LBL,,, 1)\n"
			"LBL1:\n"
			"\t; (RET,,, `0`)\n"
			"MVI A, 0\n"
			"LXI H, 22\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"RET\n"
			"\n"
			"main:\n"
			"LXI B, 0\n" // 14 - RETVAL
			"PUSH B\n" // 10, a       (13)
			"PUSH B\n" // 8 , b       (14)
			"PUSH B\n" // 6 , !temp8  (15)
			"PUSH B\n" // 4 , !temp9  (16)
			"PUSH B\n" // 2 , !temp10 (17)
			"PUSH B\n" // 0 , !temp11 (18)
			"\t; (IN,,, 13)\n"
			"IN 0\n"
			"LXI H, 10\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (IN,,, 14)\n"
			"IN 0\n"
			"LXI H, 8\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (CALL, 1,, 16)\n"
			"PUSH B\n"
			"PUSH D\n"
			"PUSH H\n"
			"PUSH PSW\n"
			"LXI B, 0\n"
			"PUSH B\n"
			"LXI B, 0\n"
			"LXI H, 22\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV C, A\n"
			"PUSH B\n"
			"LXI B, 0\n"
			"LXI H, 22\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV C, A\n"
			"PUSH B\n"
			"CALL abc\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"MOV A, B\n"
			"LXI H, 14\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP PSW\n"
			"POP H\n"
			"POP D\n"
			"POP B\n"
			"\t; (CALL, 1,, 18)\n"
			"PUSH B\n"
			"PUSH D\n"
			"PUSH H\n"
			"PUSH PSW\n"
			"LXI B, 0\n"
			"PUSH B\n"
			"LXI B, 0\n"
			"LXI H, 22\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV C, A\n"
			"PUSH B\n"
			"LXI B, 0\n"
			"LXI H, 22\n"
			"DAD SP\n"
			"MOV A, M\n"
			"MOV C, A\n"
			"PUSH B\n"
			"CALL abc\n"
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
			"POP B\n"
			"\t; (NOT, 18,, 17)\n"
			"LXI H, 0\n"
			"DAD SP\n"
			"MOV A, M\n"
			"CMA\n"
			"LXI H, 2\n"
			"DAD SP\n"
			"MOV M, A\n"
			"\t; (CALL, 1,, 15)\n"
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
			"CALL abc\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"MOV A, B\n"
			"LXI H, 16\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP PSW\n"
			"POP H\n"
			"POP D\n"
			"POP B\n"
			"\t; (OUT,,, 15)\n"
			"LXI H, 6\n"
			"DAD SP\n"
			"MOV A, M\n"
			"OUT 1\n"
			"\t; (RET,,, `0`)\n"
			"MVI A, 0\n"
			"LXI H, 14\n"
			"DAD SP\n"
			"MOV M, A\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"POP B\n"
			"RET\n",
			oss.str());
}