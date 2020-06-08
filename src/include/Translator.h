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
	std::map<Scope, std::vector<std::shared_ptr<Atom>>> _atoms;
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

	void printSymbolTable(std::ostream& stream);

	void generateAtoms(Scope scope, std::shared_ptr<Atom> atom);

	std::shared_ptr<LabelOperand> newLabel();

	void syntaxError(const std::string& message = "Error during syntax analysis");

	void lexicalError(const std::string& message);

	const std::deque<Token>& getLastLexems() const;

protected:

	void getAndCheckLexem(bool eofAcceptable = false, const std::vector<LexemType>& acceptableLexems = {});

	std::shared_ptr<MemoryOperand> checkVar(const Scope scope, const std::string& name);

	std::shared_ptr<MemoryOperand> checkFunc(const std::string& name, int len);

	void pushBackLexem();

	std::shared_ptr<RValue> E(Scope scope);

	std::shared_ptr<RValue> E1(Scope scope);

	std::shared_ptr<RValue> E1_(Scope scope, const std::string& p);

	std::shared_ptr<RValue> E2(Scope scope);

	std::shared_ptr<RValue> E3(Scope scope);

	std::shared_ptr<RValue> E3_(Scope scope, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E4(Scope scope);

	std::shared_ptr<RValue> E4_(Scope scope, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E5(Scope scope);

	std::shared_ptr<RValue> E5_(Scope scope, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E6(Scope scope);

	std::shared_ptr<RValue> E6_(Scope scope, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E7(Scope scope);

	std::shared_ptr<RValue> E7_(Scope scope, std::shared_ptr<RValue> p);

	SymbolTable::TableRecord::RecordType Type(Scope scope);

	bool InitVar(Scope scope, SymbolTable::TableRecord::RecordType recordType, const std::string& q);

	int ParamList(Scope scope);

	int ParamList_(Scope scope);

	bool DeclVarList_(Scope scope, SymbolTable::TableRecord::RecordType p);

	bool DeclareStmt_(Scope scope, SymbolTable::TableRecord::RecordType p, const std::string& q);

	bool StmtList(Scope scope);

	bool DeclareStmt(Scope scope);

	bool Stmt(Scope scope);
};

class TranslationException : public std::exception {
private:
	std::string _error;

public:
	TranslationException(std::string error);

	const char *what() const noexcept;
};

#endif //PROJECT_MICRIC2_TRANSLATOR_H