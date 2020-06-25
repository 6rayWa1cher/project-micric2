//
// Created by 6rayWa1cher and Throder-TVRS on 04.06.2020.
//

#ifndef PROJECT_MICRIC2_STRINGTABLE_H
#define PROJECT_MICRIC2_STRINGTABLE_H

#include <string>
#include <vector>
#include <memory>
#include "Atoms.h"

class StringTable {
private:
	std::vector<std::string> _strings;
public:
	const std::string& operator[](const unsigned int index) const;

	std::shared_ptr<StringOperand> add(const std::string& name);

	void printStringTable(std::ostream& stream);

    void generateStrings(std::ostream& stream) const;
};

#endif //PROJECT_MICRIC2_STRINGTABLE_H
