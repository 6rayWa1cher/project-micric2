//
// Created by Throder and 6rayWa1cher on 05.06.2020.
//
#ifndef PROJECT_MICRIC2_TRANSLATOR_H
#define PROJECT_MICRIC2_TRANSLATOR_H

#include "Atoms.h"
#include "StringTable.h"
#include "SymbolTable.h"
#include "Scanner.h"
#include <exception>
#include <queue>

class Translator {
protected:
	std::vector<std::shared_ptr<Atom>> _atoms;
	SymbolTable _symbolTable;
	StringTable _stringTable;
	Scanner _scanner;
	Token _currentLexem;

	std::deque<Token> _lastLexems;

	size_t _labelCount;
public:
	Translator(std::istream& inputStream);

	virtual void startTranslation();

	const SymbolTable& getSymbolTable() const;

	const StringTable& getStringTable() const;

	void printAtoms(std::ostream& stream);

	void generateAtoms(std::shared_ptr<Atom> atom);

	std::shared_ptr<LabelOperand> newLabel();

	void syntaxError(const std::string& message);

	void lexicalError(const std::string& message);

	const std::deque<Token>& getLastLexems() const;

protected:
	std::shared_ptr<RValue> E();

	std::shared_ptr<RValue> E1();

	std::shared_ptr<RValue> E1_(const std::string& p);

	std::shared_ptr<RValue> E2();

	std::shared_ptr<RValue> E3();

	std::shared_ptr<RValue> E3_(std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E4();

	std::shared_ptr<RValue> E4_(std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E5();

	std::shared_ptr<RValue> E5_(std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E6();

	std::shared_ptr<RValue> E6_(std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E7();

	std::shared_ptr<RValue> E7_(std::shared_ptr<RValue> p);

	void getAndCheckLexem(bool eofAcceptable = false);

	void pushBackLexem();
};

class TranslationException : public std::exception {
private:
	std::string _error;

public:
	TranslationException(std::string error);

	const char *what() const noexcept;
};

#endif //PROJECT_MICRIC2_TRANSLATOR_H