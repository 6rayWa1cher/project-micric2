//
// Created by 6rayWa1cher on 04.06.2020.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include "../src/include/Atoms.h"
#include "../src/include/SymbolTable.h"
#include "../src/include/StringTable.h"

TEST(StringTableTests, Overall) {
	StringTable table;
	auto string = "Hello world!";
	std::shared_ptr<StringOperand> ptr = table.add(string);
	ASSERT_FALSE(!ptr);
	ASSERT_EQ(std::string("0{") + string + "}", ptr->toString());
	ASSERT_EQ(string, table[0]);

	auto string2 = "Hello world again!";
	std::shared_ptr<StringOperand> ptr2 = table.add(string2);
	ASSERT_FALSE(!ptr2);
	ASSERT_EQ(std::string("1{") + string2 + "}", ptr2->toString());
	ASSERT_EQ(string2, table[1]);
}

TEST(SymbolTableTests, Overall) {
	SymbolTable table;
	auto string = "meaningOfLife";
	std::shared_ptr<MemoryOperand> ptr = table.add(string);
	ASSERT_FALSE(!ptr);
	ASSERT_EQ(std::string("0[") + string + "]", ptr->toString());
	ASSERT_EQ(string, table[0]);

	auto string2 = "meaningOfLife2";
	std::shared_ptr<MemoryOperand> ptr2 = table.add(string2);
	ASSERT_FALSE(!ptr2);
	ASSERT_EQ(std::string("1[") + string2 + "]", ptr2->toString());
	ASSERT_EQ(string2, table[1]);
}

TEST(AtomTests, MemoryOperand) {
	SymbolTable table;
	auto string = "some symbol";
	table.add(string);
	MemoryOperand memoryOperand(0, &table);
	ASSERT_EQ(std::string("0[") + string + "]", memoryOperand.toString());

	auto string2 = "some symbol2";
	table.add(string2);
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
	ASSERT_EQ("2020", labelOperand.toString());
}

#pragma clang diagnostic pop