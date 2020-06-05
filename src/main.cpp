#include <iostream>
#include <sstream>
#include "Scanner.h"
#include "SymbolTable.h"

int main() {
	std::cout << "Hello, World!" << std::endl;
	std::istringstream istringstream("1 + 2");
	Scanner scanner(istringstream);
	Token token = scanner.getNextToken();
	std::cout << token.value() << std::endl;

	SymbolTable cTable;
	const std::string hname = "ADD";
	std::shared_ptr<MemoryOperand> s = cTable.add("x");
	std::shared_ptr<MemoryOperand> l = cTable.add("y");
	std::cout << s->toString() << std::endl;
	std::cout << l->toString() << std::endl;

	BinaryOpAtom binaryAtom(hname, s, l, s);

	std::cout << binaryAtom.toString() << std::endl;

	return 0;
}
