//
// Created by 6rayWa1cher on 04.06.2020.
//

#include <gtest/gtest.h>
#include "../src/include/meaning.h"

TEST(ScannerTests, EmptyFile) {
	ASSERT_EQ(42, meaningOfLife());
}