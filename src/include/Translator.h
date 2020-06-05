#ifndef PROJECT_MICRIC2_TRANSLATOR_H
#define PROJECT_MICRIC2_TRANSLATOR_H

#include "Atoms.h"
#include "StringTable.h"
#include "SymbolTable.h"
#include "Scanner.h"
#include <exception>

class Translator {
private:
	std::vector<std::shared_ptr<Atom>> _atoms;
	SymbolTable _symbolTable;
	StringTable _stringTable;
	Scanner _scanner;
	Token _currentLexem;

	size_t labelCount;
public:
	Translator(std::istream& inputStream);

	void printAtoms(std::ostream& stream);

	void generateAtoms(std::shared_ptr<Atom> atom);

	std::shared_ptr<LabelOperand> newLabel();

	void syntaxError(const std::string& message);

	void lexicalError(const std::string& message);
};

class TranslatorException : public std::exception {
private:
	std::string _error;

public:
	TranslatorException(std::string error);

	const char* what() const noexcept;
};

#endif //PROJECT_MICRIC2_TRANSLATOR_H