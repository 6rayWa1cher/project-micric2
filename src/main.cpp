#include <iostream>
#include <sstream>
#include "Scanner.h"

int main() {
	std::cout << "Hello, World!" << std::endl;
	std::istringstream istringstream("1 + 2");
	Scanner scanner(istringstream);
	Token token = scanner.getNextToken();
	std::cout << token.value();
	return 0;
}
