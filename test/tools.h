//
// Created by 6rayWa1cher on 08.06.2020.
//

#ifndef PROJECT_MICRIC2_TOOLS_H
#define PROJECT_MICRIC2_TOOLS_H

#include <vector>
#include <string>
#include "../src/include/SymbolTable.h"

std::vector<std::string> split(const std::string& string, char delimiter);

std::vector<std::string> getAtomsExpression(const std::string& s, std::vector<std::string> vars, Scope scope = 1337);

#endif //PROJECT_MICRIC2_TOOLS_H
