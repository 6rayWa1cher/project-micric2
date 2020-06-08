//
// Created by Throder and 6rayWa1cher on 05.06.2020.
//

#include "../include/Translator.h"

#include <algorithm>
#include <utility>
#include <sstream>
#include "../include/Atoms.h"
#include "../include/StringTable.h"
#include "../include/SymbolTable.h"


Translator::Translator(std::istream& inputStream) : _scanner(Scanner(inputStream)) {
	_labelCount = 0;
}

void Translator::printAtoms(std::ostream& stream) {
	for (const auto& pair : _atoms) {
		Scope scope = pair.first;
		for (const auto& atom : pair.second) {
			stream << scope << '\t' << atom->toString() << std::endl;
		}
	}
}

void Translator::printSymbolTable(std::ostream& stream) {
	_symbolTable.printSymbolTable(stream);
}

void Translator::generateAtoms(Scope scope, std::shared_ptr<Atom> atom) {
	_atoms.emplace(scope, std::vector<std::shared_ptr<Atom>>());
	_atoms[scope].push_back(atom);
}

std::shared_ptr<LabelOperand> Translator::newLabel() {
	auto label = std::make_shared<LabelOperand>(_labelCount);
	_labelCount++;
	return label;
}

void Translator::syntaxError(const std::string& message) {
	size_t rowPos = _scanner.getRowPos();
	std::string error = "SYNTAX ERROR: " + message + "\n" +
	                    "Appeared on the " + std::to_string(rowPos) + " line.\n";
	error += "Current lexem: [" + _currentLexem.toString() + "]\n";
	error += getLastLexems();
	throw TranslationException(error);
}

void Translator::lexicalError(const std::string& message) {
	size_t rowPos = _scanner.getRowPos();
	std::string error = "LEXICAL ERROR: [" + message + "]\n" +
	                    "Appeared on the " + std::to_string(rowPos) + " line.\n";
	error += getLastLexems();
	throw TranslationException(error);
}

void Translator::getAndCheckLexem(bool eofAcceptable, const std::vector<LexemType>& acceptableLexems) {
	_currentLexem = _scanner.getNextToken();
	_lastLexems.push_back(_currentLexem);
	while (_lastLexems.back().type() != LexemType::eof && _lastLexems.size() > 4) {
		_lastLexems.pop_front();
	}
	LexemType lexemType = _currentLexem.type();
	if (lexemType == LexemType::error) {
		lexicalError(_currentLexem.toString());
	}
	if (!eofAcceptable && lexemType == LexemType::eof) {
		syntaxError("Reached EOF before end of a syntax analysis");
	}
	if (!acceptableLexems.empty() &&
	    std::find(acceptableLexems.begin(), acceptableLexems.end(), lexemType) == acceptableLexems.end()) {
		std::stringstream ss;
		ss << "Not matching lexem type: expected - ";
		for (int i = 0; i < acceptableLexems.size(); ++i) {
			const LexemType accLexem = acceptableLexems[i];
			if (i == 0) {
				ss << '[' << enToStr(accLexem) << ']';
			} else if (i + 1 == acceptableLexems.size()) {
				ss << " or " << '[' << enToStr(accLexem) << ']';
			} else {
				ss << ", " << '[' << enToStr(accLexem) << ']';
			}
		}
		ss << ", got - " << '[' << _currentLexem.toString() << ']';
		syntaxError(ss.str());
	}
}


void Translator::pushBackLexem() {
	_scanner.pushBack(_currentLexem);
	_lastLexems.pop_back();
}


std::shared_ptr<RValue> Translator::E(Scope scope) {
	// 1_1
	auto q = E7(scope);
	if (!q) {
		syntaxError("Error during syntax analysis on rule E7");
		return nullptr;
	}
	return q;
}

std::shared_ptr<RValue> Translator::E7(Scope scope) {
	// 1_2
	auto q = E6(scope);
	if (!q) {
		syntaxError("Error during syntax analysis on rule E6");
		return nullptr;
	}
	auto s = E7_(scope, q);
	if (!s) {
		syntaxError("Error during syntax analysis on rule E7_");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E7_(Scope scope, std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opor) { // 1_3
		auto r = E6(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E6");
			return nullptr;
		}
		auto s = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("OR", p, r, s));
		auto t = E7_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E7_");
			return nullptr;
		}
		return t;
	} else { // 1_4
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E6(Scope scope) {
	// 1_5
	auto q = E5(scope);
	if (!q) {
		syntaxError("Error during syntax analysis on rule E5");
		return nullptr;
	}
	auto s = E6_(scope, q);
	if (!s) {
		syntaxError("Error during syntax analysis on rule E6_");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E6_(Scope scope, std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opand) { // 1_6
		auto r = E5(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E5");
			return nullptr;
		}
		auto s = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("AND", p, r, s));
		auto t = E6_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E6_");
			return nullptr;
		}
		return t;
	} else { // 1_7
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E5(Scope scope) {
	// 1_8
	auto q = E4(scope);
	if (!q) {
		syntaxError("Error during syntax analysis on rule E4");
		return nullptr;
	}
	auto s = E5_(scope, q);
	if (!s) {
		syntaxError("Error during syntax analysis on rule E5_");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E5_(Scope scope, std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (
			_currentLexem.type() == LexemType::opeq ||
			_currentLexem.type() == LexemType::opne ||
			_currentLexem.type() == LexemType::opgt ||
			_currentLexem.type() == LexemType::oplt ||
			_currentLexem.type() == LexemType::ople
			) { // 1_9-13
		auto savedLexem = _currentLexem;
		auto r = E4(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E4");
			return nullptr;
		}
		auto s = _symbolTable.alloc(scope);
		auto l = this->newLabel();
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s));
		switch (savedLexem.type()) {
			case LexemType::opeq:
				generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("EQ", p, r, l));
				break;
			case LexemType::opne:
				generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("NE", p, r, l));
				break;
			case LexemType::opgt:
				generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("GT", p, r, l));
				break;
			case LexemType::oplt:
				generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("LT", p, r, l));
				break;
			case LexemType::ople:
				generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("LE", p, r, l));
				break;
			default:
				syntaxError("Unexpected operator on rule E5_");
		}
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s));
		generateAtoms(scope, std::make_shared<LabelAtom>(l));
		return s;
	} else { // 1_14
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E4(Scope scope) {
	// 1_15
	auto q = E3(scope);
	if (!q) {
		syntaxError("Error during syntax analysis on rule E3");
		return nullptr;
	}
	auto s = E4_(scope, q);
	if (!s) {
		syntaxError("Error during syntax analysis on rule E4_");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E4_(Scope scope, std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opplus) { // 1_16
		auto r = E3(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E3");
			return nullptr;
		}
		auto s = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("ADD", p, r, s));
		auto t = E4_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E4_");
			return nullptr;
		}
		return t;
	} else if (_currentLexem.type() == LexemType::opminus) { // 1_17
		auto r = E3(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E3");
			return nullptr;
		}
		auto s = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("SUB", p, r, s));
		auto t = E4_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E4_");
			return nullptr;
		}
		return t;
	} else {// 1_18
		pushBackLexem();
		return p;
	}
}


std::shared_ptr<RValue> Translator::E3(Scope scope) {
	// 1_19
	auto q = E2(scope);
	if (!q) {
		syntaxError("Error during syntax analysis on rule E2");
		return nullptr;
	}
	auto s = E3_(scope, q);
	if (!s) {
		syntaxError("Error during syntax analysis on rule E3_");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E3_(Scope scope, std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opmult) { // 1_20
		auto r = E2(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E2");
			return nullptr;
		}
		auto s = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("MUL", p, r, s));
		auto t = E3_(scope, s);
		return t;
	} else { // 1_21
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E2(Scope scope) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opnot) { // 1_22
		auto q = E1(scope);
		if (!q) {
			syntaxError("Error during syntax analysis on rule E1");
			return nullptr;
		}
		auto r = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("NOT", q, r));
		return r;
	} else { // 1_23
		pushBackLexem();
		return E1(scope);
	}
}

std::shared_ptr<RValue> Translator::E1(Scope scope) {
	getAndCheckLexem(false, {LexemType::lpar, LexemType::num, LexemType::chr, LexemType::opinc, LexemType::id});
	if (_currentLexem.type() == LexemType::lpar) { // 1_24
		auto q = this->E(scope);
		if (!q) {
			syntaxError("Error during syntax analysis on rule E");
			return nullptr;
		}
		getAndCheckLexem(false, {LexemType::rpar});
		return q;
	}
	if (_currentLexem.type() == LexemType::num) { // 1_25
		return std::make_shared<NumberOperand>(_currentLexem.value());
	}
	if (_currentLexem.type() == LexemType::chr) { // 1_26
		return std::make_shared<NumberOperand>(_currentLexem.value());
	}
	if (_currentLexem.type() == LexemType::opinc) { // 1_27
		getAndCheckLexem(false, {LexemType::id});
		auto name = _currentLexem.str();
		auto q = checkVar(scope, name);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("ADD", q, std::make_shared<NumberOperand>(1), q));
		return q;
	}
	if (_currentLexem.type() == LexemType::id) { // 1_28
		auto name = _currentLexem.str();
		auto s = E1_(scope, name);
		if (!s) {
			syntaxError("Error during syntax analysis on rule E1_");
			return nullptr;
		}
		return s;
	}
	syntaxError();
	return nullptr;
}


std::shared_ptr<RValue> Translator::E1_(Scope scope, const std::string& p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opinc) { // 1_29
		auto s = checkVar(scope, p);
		auto r = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("MOV", s, r));
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("ADD", s, std::make_shared<NumberOperand>(1), s));
		return r;
	}

	// TODO: 1_30

	// 1_31
	pushBackLexem();
	return checkVar(scope, p);
}

bool Translator::DeclareStmt(Scope scope) {
	auto p = Type(scope);
	if (p == SymbolTable::TableRecord::RecordType::unknown) {
		syntaxError();
		return false;
	}
	getAndCheckLexem(false, {LexemType::id});
	auto name = _currentLexem.str();
	if (!DeclareStmt_(scope, p, name)) {
		syntaxError();
		return false;
	}
	return true;
}

bool Translator::DeclareStmt_(Scope scope, SymbolTable::TableRecord::RecordType p, const std::string& q) {
	getAndCheckLexem(false, {LexemType::lpar, LexemType::opassign, LexemType::comma, LexemType::semicolon});
	if (_currentLexem.type() == LexemType::lpar) { // 2_2
		if (scope > -1) {
			syntaxError("Local functions are not allowed");
			return false;
		}
		auto func = _symbolTable.addFunc(q, p, 0);
		auto newScope = func->index();
		int n = ParamList(newScope);
		if (n < -1) {
			syntaxError();
			return false;
		}
		_symbolTable.changeFuncLength(q, n);
		getAndCheckLexem(false, {LexemType::rpar});
		getAndCheckLexem(false, {LexemType::lbrace});
		if (!StmtList(newScope)) {
			syntaxError();
			return false;
		}
		getAndCheckLexem(false, {LexemType::rbrace});
		generateAtoms(newScope, std::make_shared<RetAtom>(std::make_shared<NumberOperand>(0)));
		return true;
	} else if (_currentLexem.type() == LexemType::opassign) { // 2_3
		getAndCheckLexem(false, {LexemType::num});
		_symbolTable.addVar(q, scope, p, _currentLexem.value());
		if (!DeclVarList_(scope, p)) {
			syntaxError();
			return false;
		}
		getAndCheckLexem(false, {LexemType::semicolon});
		return true;
	} else { // 2_4
		pushBackLexem();
		_symbolTable.addVar(q, scope, p);
		if (!DeclVarList_(scope, p)) {
			syntaxError();
			return false;
		}
		getAndCheckLexem(false, {LexemType::semicolon});
		return true;
	}
}

SymbolTable::TableRecord::RecordType Translator::Type(Scope scope) {
	getAndCheckLexem(false, {LexemType::kwchar, LexemType::kwint});
	if (_currentLexem.type() == LexemType::kwchar) { // 2_5
		return SymbolTable::TableRecord::RecordType::chr;
	} else if (_currentLexem.type() == LexemType::kwint) { // 2_6
		return SymbolTable::TableRecord::RecordType::integer;
	} else {
		syntaxError();
		return SymbolTable::TableRecord::RecordType::unknown;
	}
}

bool Translator::DeclVarList_(Scope scope, SymbolTable::TableRecord::RecordType p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::comma) { // 2_7
		getAndCheckLexem(false, {LexemType::id});
		auto name = _currentLexem.str();
		if (!InitVar(scope, p, name)) {
			syntaxError();
			return false;
		}
		if (!DeclVarList_(scope, p)) {
			syntaxError();
			return false;
		}
		return true;
	}
	// 2_8
	pushBackLexem();
	return true;
}

bool Translator::InitVar(Scope scope, SymbolTable::TableRecord::RecordType p, const std::string& q) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opassign) { // 2_9
		getAndCheckLexem(false, {LexemType::chr, LexemType::num});
		_symbolTable.addVar(q, scope, p, _currentLexem.value());
		return true;
	}
	// 2_10
	_symbolTable.addVar(q, scope, p);
	pushBackLexem();
	return true;
}

int Translator::ParamList(Scope scope) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::kwint ||
	    _currentLexem.type() == LexemType::kwchar) { // 2_11
		pushBackLexem();
		auto q = Type(scope);
		if (q == SymbolTable::TableRecord::RecordType::unknown) {
			syntaxError("Unknown type of variable");
			return -1;
		}
		getAndCheckLexem(false, {LexemType::id});
		auto name = _currentLexem.str();
		_symbolTable.addVar(name, scope, q);
		int s = ParamList_(scope);
		if (s == -1) {
			syntaxError();
			return -1;
		}
		return s + 1;
	}
	// 2_12
	pushBackLexem();
	return 0;
}

int Translator::ParamList_(Scope scope) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::comma) { // 2_13
		auto q = Type(scope);
		if (q == SymbolTable::TableRecord::RecordType::unknown) {
			syntaxError("Unknown type of variable");
			return -1;
		}
		getAndCheckLexem(false, {LexemType::id});
		auto name = _currentLexem.str();
		int s = ParamList_(scope);
		if (s == -1) {
			syntaxError();
			return -1;
		}
		_symbolTable.addVar(name, scope, q);
		return s + 1;
	}
	// 2_14
	pushBackLexem();
	return 0;
}

bool Translator::StmtList(Scope scope) {
	getAndCheckLexem(true);
	std::vector<LexemType> continueVector = {
			LexemType::kwchar, LexemType::kwint, // -> DeclareStmt
			LexemType::id, // -> AssignOrCallOp
			LexemType::kwwhile, // -> WhileOp
			LexemType::kwfor, // -> ForOp
			LexemType::kwif, // -> IfOp
			LexemType::kwswitch, // -> SwitchOp
			LexemType::kwin, // -> IOP
			LexemType::kwout, // -> OOp
			LexemType::lbrace, // -> 2_25
			LexemType::kwreturn, // -> 2_26
			LexemType::semicolon // -> 2_27
	};
	if (std::find(continueVector.begin(), continueVector.end(), _currentLexem.type()) != continueVector.end()) {
		pushBackLexem();
		if (!Stmt(scope)) {
			syntaxError();
			return false;
		}
		if (!StmtList(scope)) {
			syntaxError();
			return false;
		}
		return true;
	} else {
		pushBackLexem();
		return true;
	}
}

bool Translator::Stmt(Scope scope) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::kwchar || _currentLexem.type() == LexemType::kwint) { // 2_17
		pushBackLexem();
		if (!DeclareStmt(scope)) {
			syntaxError();
			return false;
		}
		return true;
	}
	if (scope == GLOBAL_SCOPE) {
		syntaxError("Operator usage outside of any function");
		return false;
	}
	// TODO: 2_18-2_27
	syntaxError();
	return false;
}

void Translator::startTranslation() {
	StmtList(GLOBAL_SCOPE);
	getAndCheckLexem(true);
	if (_currentLexem != LexemType::eof) {
		syntaxError("Syntax analysis was completed, but an additional lexeme appeared");
	}
}

const SymbolTable& Translator::getSymbolTable() const {
	return _symbolTable;
}

const StringTable& Translator::getStringTable() const {
	return _stringTable;
}

std::string Translator::getLastLexems() {
	std::string lexemStr = "Last correctly read lexemes: ";
	for (int i = 0; i < 4 - int(_lastLexems.size()); i++) {
		lexemStr += "[nothing]";
		if (i < 2) lexemStr += ", ";
	}
	while (_lastLexems.size() > 1) {
		lexemStr += "[" + _lastLexems.front().toString() + "]";
		if (_lastLexems.size() != 2) lexemStr += ", ";
		_lastLexems.pop_front();
	}
	lexemStr += "\n";
	return lexemStr;
}

std::shared_ptr<MemoryOperand> Translator::checkVar(Scope scope, const std::string& name) {
	auto out = _symbolTable.checkVar(scope, name);
	if (!out) {
		syntaxError("An unknown or a non-var name reference \"" + name + "\"");
		return nullptr;
	}
	return out;
}

std::shared_ptr<MemoryOperand> Translator::checkFunc(const std::string& name, int len) {
	auto out = _symbolTable.checkFunc(name, len);
	if (!out) {
		syntaxError("An unknown or a non-func name reference \"" + name + "\"");
		return nullptr;
	}
	return out;
}


TranslationException::TranslationException(std::string error) : _error(std::move(error)) {}

const char *TranslationException::what() const noexcept {
	return _error.c_str();
}
