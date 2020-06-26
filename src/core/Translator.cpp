//
// Created by Throder-TVRS and 6rayWa1cher on 05.06.2020.
//

#include "../include/Translator.h"

#include <algorithm>
#include <sstream>
#include <utility>


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

void Translator::printStringTable(std::ostream& stream) {
	_stringTable.printStringTable(stream);
}

void Translator::generateAtoms(Scope scope, const std::shared_ptr<Atom>& atom) {
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
	error += "Current lexem: [" + _currentLexeme.toString() + "]\n";
	error += getLastLexemes();
	throw TranslationException(error);
}

void Translator::lexicalError(const std::string& message) {
	size_t rowPos = _scanner.getRowPos();
	std::string error = "LEXICAL ERROR: [" + message + "]\n" +
	                    "Appeared on the " + std::to_string(rowPos) + " line.\n";
	error += getLastLexemes();
	throw TranslationException(error);
}

void Translator::getAndCheckLexeme(bool eofAcceptable, const std::vector<LexemType>& acceptableLexems) {
	_currentLexeme = _scanner.getNextToken();
	LexemType lexemType = _currentLexeme.type();
	if (lexemType == LexemType::error) {
		lexicalError(_currentLexeme.toString());
	}
	if (!eofAcceptable && lexemType == LexemType::eof) {
		syntaxError("Reached EOF before the end of syntax analysis");
	}
	if (!acceptableLexems.empty() &&
	    std::find(acceptableLexems.begin(), acceptableLexems.end(), lexemType) == acceptableLexems.end()) {
		std::stringstream ss;
		ss << "Not matching lexeme type: expected - ";
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
		ss << ", got - " << '[' << _currentLexeme.toString() << ']';
		syntaxError(ss.str());
	}
	_lastLexemes.push_back(_currentLexeme);
	while (_lastLexemes.back().type() != LexemType::eof && _lastLexemes.size() > 4) {
		_lastLexemes.pop_front();
	}
}


void Translator::pushBackLexeme() {
	_scanner.pushBack(_currentLexeme);
	_lastLexemes.pop_back();
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
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::opor) { // 1_3
		auto s = _symbolTable.alloc(scope);
		auto r = E6(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E6");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("OR", p, r, s));
		auto t = E7_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E7_");
			return nullptr;
		}
		return t;
	} else { // 1_4
		pushBackLexeme();
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
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::opand) { // 1_6
		auto s = _symbolTable.alloc(scope);
		auto r = E5(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E5");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("AND", p, r, s));
		auto t = E6_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E6_");
			return nullptr;
		}
		return t;
	} else { // 1_7
		pushBackLexeme();
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
	getAndCheckLexeme(true);
	if (
			_currentLexeme.type() == LexemType::opeq ||
			_currentLexeme.type() == LexemType::opne ||
			_currentLexeme.type() == LexemType::opgt ||
			_currentLexeme.type() == LexemType::oplt ||
			_currentLexeme.type() == LexemType::ople
			) { // 1_9-13
		auto savedLexeme = _currentLexeme;
		auto s = _symbolTable.alloc(scope);
		auto l = this->newLabel();
		auto r = E4(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E4");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s));
		switch (savedLexeme.type()) {
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
				syntaxError("An unexpected operator on rule E5_ appeared");
		}
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s));
		generateAtoms(scope, std::make_shared<LabelAtom>(l));
		return s;
	} else { // 1_14
		pushBackLexeme();
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
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::opplus) { // 1_16
		auto s = _symbolTable.alloc(scope);
		auto r = E3(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E3");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("ADD", p, r, s));
		auto t = E4_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E4_");
			return nullptr;
		}
		return t;
	} else if (_currentLexeme.type() == LexemType::opminus) { // 1_17
		auto s = _symbolTable.alloc(scope);
		auto r = E3(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E3");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("SUB", p, r, s));
		auto t = E4_(scope, s);
		if (!t) {
			syntaxError("Error during syntax analysis on rule E4_");
			return nullptr;
		}
		return t;
	} else {// 1_18
		pushBackLexeme();
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
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::opmult) { // 1_20
		auto s = _symbolTable.alloc(scope);
		auto r = E2(scope);
		if (!r) {
			syntaxError("Error during syntax analysis on rule E2");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("MUL", p, r, s));
		auto t = E3_(scope, s);
		return t;
	} else { // 1_21
		pushBackLexeme();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E2(Scope scope) {
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::opnot) { // 1_22
		auto r = _symbolTable.alloc(scope);
		auto q = E1(scope);
		if (!q) {
			syntaxError("Error during syntax analysis on rule E1");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("NOT", q, r));
		return r;
	} else { // 1_23
		pushBackLexeme();
		return E1(scope);
	}
}

std::shared_ptr<RValue> Translator::E1(Scope scope) {
	getAndCheckLexeme(false, {LexemType::lpar, LexemType::num, LexemType::chr, LexemType::opinc, LexemType::id});
	if (_currentLexeme.type() == LexemType::lpar) { // 1_24
		auto q = this->E(scope);
		if (!q) {
			syntaxError("Error during syntax analysis on rule E");
			return nullptr;
		}
		getAndCheckLexeme(false, {LexemType::rpar});
		return q;
	}
	if (_currentLexeme.type() == LexemType::num) { // 1_25
		return std::make_shared<NumberOperand>(_currentLexeme.value());
	}
	if (_currentLexeme.type() == LexemType::chr) { // 1_26
		return std::make_shared<NumberOperand>(_currentLexeme.value());
	}
	if (_currentLexeme.type() == LexemType::opinc) { // 1_27
		getAndCheckLexeme(false, {LexemType::id});
		auto name = _currentLexeme.str();
		auto q = checkVar(scope, name);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("ADD", q, std::make_shared<NumberOperand>(1), q));
		return q;
	}
	if (_currentLexeme.type() == LexemType::id) { // 1_28
		auto name = _currentLexeme.str();
		auto s = E1_(scope, name);
		if (!s) {
			syntaxError("Error during syntax analysis on rule E1_");
			return nullptr;
		}
		return s;
	}
	syntaxError("An unexpected lexeme on rule E1 appeared");
	return nullptr;
}


std::shared_ptr<RValue> Translator::E1_(Scope scope, const std::string& p) {
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::opinc) { // 1_29
		auto s = checkVar(scope, p);
		auto r = _symbolTable.alloc(scope);
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("MOV", s, r));
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("ADD", s, std::make_shared<NumberOperand>(1), s));
		return r;
	} else if (_currentLexeme.type() == LexemType::lpar) { // 1_30
		auto r = _symbolTable.alloc(scope);
		int n = ArgList(scope);
		if (n == -1) {
			syntaxError("Error reading argument in ArgList");
			return nullptr;
		}
		auto s = checkFunc(p, n);
		if (!s) {
			syntaxError("Error on rule E1_; The number of function arguments does not match the definition");
			return nullptr;
		}
		getAndCheckLexeme(false, {LexemType::rpar});
		generateAtoms(scope, std::make_shared<CallAtom>(s, r));
		return r;
	}

	// 1_31
	auto z = checkVar(scope, p);
	pushBackLexeme();
	return z;
}

int Translator::ArgList(Scope scope) {
	getAndCheckLexeme(false);
	if (_currentLexeme.type() == LexemType::opnot ||
	    _currentLexeme.type() == LexemType::lpar ||
	    _currentLexeme.type() == LexemType::num ||
	    _currentLexeme.type() == LexemType::chr ||
	    _currentLexeme.type() == LexemType::opinc ||
	    _currentLexeme.type() == LexemType::id) {     // 1_32

		pushBackLexeme();
		auto p = E(scope);
		if (!p) {
			syntaxError("Error reading argument in ArgList");
			return -1;
		}
		generateAtoms(scope, std::make_shared<ParamAtom>(p));
		auto m = ArgList_(scope);
		if (m == -1) {
			syntaxError("Error reading argument in ArgList_");
			return -1;
		}
		return m + 1;
	}
	// 1_33
	pushBackLexeme();
	return 0;
}

int Translator::ArgList_(Scope scope) {
	getAndCheckLexeme(false);
	if (_currentLexeme.type() == LexemType::comma) { // 1_34
		auto p = E(scope);
		if (!p) {
			syntaxError("Error reading argument in ArgList_");
			return -1;
		}
		generateAtoms(scope, std::make_shared<ParamAtom>(p));
		auto m = ArgList_(scope);
		if (m == -1) {
			syntaxError("Error reading argument in ArgList_");
			return -1;
		}
		return m + 1;
	}
	// 1_35
	pushBackLexeme();
	return 0;
}

bool Translator::DeclareStmt(Scope scope) {
	auto p = Type(scope);
	if (p == SymbolTable::TableRecord::RecordType::unknown) {
		syntaxError("Unknown data type in DeclareStmt");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::id});
	auto name = _currentLexeme.str();
	if (!DeclareStmt_(scope, p, name)) {
		syntaxError("Error during syntax analysis on rule DeclareStmt");
		return false;
	}
	return true;
}

bool Translator::DeclareStmt_(Scope scope, SymbolTable::TableRecord::RecordType p, const std::string& q) {
	getAndCheckLexeme(false, {LexemType::lpar, LexemType::opassign, LexemType::comma, LexemType::semicolon});
	if (_currentLexeme.type() == LexemType::lpar) { // 2_2
		if (scope > -1) {
			syntaxError("Local functions are not allowed");
			return false;
		}
		auto func = _symbolTable.addFunc(q, p, 0);
		auto newScope = func->index();
		int n = ParamList(newScope);
		if (n == -1) {
			syntaxError("Error while reading parameters in ParamList");
			return false;
		}
		_symbolTable.changeFuncLength(q, n);
		getAndCheckLexeme(false, {LexemType::rpar});
		getAndCheckLexeme(false, {LexemType::lbrace});
		if (!StmtList(newScope)) {
			syntaxError("Error in the definition of function body on StmtList");
			return false;
		}
		getAndCheckLexeme(false, {LexemType::rbrace});
		generateAtoms(newScope, std::make_shared<RetAtom>(std::make_shared<NumberOperand>(0)));
		return true;
	} else if (_currentLexeme.type() == LexemType::opassign) { // 2_3
		getAndCheckLexeme(false, {LexemType::num});
		_symbolTable.addVar(q, scope, p, _currentLexeme.value());
		if (!DeclVarList_(scope, p)) {
			syntaxError("Error while declaring variables on DeclVarList_");
			return false;
		}
		getAndCheckLexeme(false, {LexemType::semicolon});
		return true;
	} else { // 2_4
		pushBackLexeme();
		_symbolTable.addVar(q, scope, p);
		if (!DeclVarList_(scope, p)) {
			syntaxError("Error while declaring variables on DeclVarList_");
			return false;
		}
		getAndCheckLexeme(false, {LexemType::semicolon});
		return true;
	}
}

SymbolTable::TableRecord::RecordType Translator::Type(Scope scope) {
	getAndCheckLexeme(false, {LexemType::kwchar, LexemType::kwint});
	if (_currentLexeme.type() == LexemType::kwchar) { // 2_5
		return SymbolTable::TableRecord::RecordType::chr;
	} else if (_currentLexeme.type() == LexemType::kwint) { // 2_6
		return SymbolTable::TableRecord::RecordType::integer;
	} else {
		syntaxError("An unknown lexeme type appeared");
		return SymbolTable::TableRecord::RecordType::unknown;
	}
}

bool Translator::DeclVarList_(Scope scope, SymbolTable::TableRecord::RecordType p) {
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::comma) { // 2_7
		getAndCheckLexeme(false, {LexemType::id});
		auto name = _currentLexeme.str();
		if (!InitVar(scope, p, name)) {
			syntaxError("Error during syntax analysis on rule InitVar");
			return false;
		}
		if (!DeclVarList_(scope, p)) {
			syntaxError("Error while declaring variables on DeclVarList_");
			return false;
		}
		return true;
	}
	// 2_8
	pushBackLexeme();
	return true;
}

bool Translator::InitVar(Scope scope, SymbolTable::TableRecord::RecordType p, const std::string& q) {
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::opassign) { // 2_9
		getAndCheckLexeme(false, {LexemType::chr, LexemType::num});
		_symbolTable.addVar(q, scope, p, _currentLexeme.value());
		return true;
	}
	// 2_10
	_symbolTable.addVar(q, scope, p);
	pushBackLexeme();
	return true;
}

int Translator::ParamList(Scope scope) {
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::kwint ||
	    _currentLexeme.type() == LexemType::kwchar) { // 2_11
		pushBackLexeme();
		auto q = Type(scope);
		if (q == SymbolTable::TableRecord::RecordType::unknown) {
			syntaxError("An unknown type of variable");
			return -1;
		}
		getAndCheckLexeme(false, {LexemType::id});
		auto name = _currentLexeme.str();
		_symbolTable.addVar(name, scope, q);
		int s = ParamList_(scope);
		if (s == -1) {
			syntaxError("Error while reading parameters in ParamList_");
			return -1;
		}
		return s + 1;
	}
	// 2_12
	pushBackLexeme();
	return 0;
}

int Translator::ParamList_(Scope scope) {
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::comma) { // 2_13
		auto q = Type(scope);
		if (q == SymbolTable::TableRecord::RecordType::unknown) {
			syntaxError("An unknown type of variable");
			return -1;
		}
		getAndCheckLexeme(false, {LexemType::id});
		auto name = _currentLexeme.str();
		_symbolTable.addVar(name, scope, q);
		int s = ParamList_(scope);
		if (s == -1) {
			syntaxError("Error while reading parameters in ParamList_");
			return -1;
		}
		return s + 1;
	}
	// 2_14
	pushBackLexeme();
	return 0;
}

bool Translator::StmtList(Scope scope) {
	getAndCheckLexeme(true);
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
	if (std::find(continueVector.begin(), continueVector.end(), _currentLexeme.type()) != continueVector.end()) {
		pushBackLexeme();
		if (!Stmt(scope)) {
			syntaxError("Error during syntax analysis on rule Stmt");
			return false;
		}
		if (!StmtList(scope)) {
			syntaxError("Error during syntax analysis on rule StmtList");
			return false;
		}
		return true;
	} else {
		pushBackLexeme();
		return true;
	}
}

bool Translator::Stmt(Scope scope) {
	getAndCheckLexeme(true);
	if (_currentLexeme.type() == LexemType::kwchar || _currentLexeme.type() == LexemType::kwint) { // 2_17
		pushBackLexeme();
		if (!DeclareStmt(scope)) {
			syntaxError("Error during syntax analysis on rule DeclareStmt");
			return false;
		}
		return true;
	}
	if (scope == GLOBAL_SCOPE) {
		syntaxError("The operator is outside of any function");
		return false;
	}
	if (_currentLexeme.type() == LexemType::id) { // 2_18
		if (!AssignOrCallOp(scope)) {
			syntaxError("Error during syntax analysis on rule AssignOrCallOp");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::kwwhile) { // 2_19
		if (!WhileOp(scope)) {
			syntaxError("Error during syntax analysis on rule WhileOp");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::kwfor) { // 2_20
		if (!ForOp(scope)) {
			syntaxError("Error during syntax analysis on rule ForOp");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::kwif) { // 2_21
		if (!IfOp(scope)) {
			syntaxError("Error during syntax analysis on rule IfOp");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::kwswitch) { // 2_22
		if (!SwitchOp(scope)) {
			syntaxError("Error during syntax analysis on rule SwitchOp");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::kwin) {  // 2_23
		if (!IOp(scope)) {
			syntaxError("Error during syntax analysis on rule IOp");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::kwout) {  // 2_24
		if (!OOp(scope)) {
			syntaxError("Error during syntax analysis on rule OOp");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::lbrace) {    // 2_25
		if (!StmtList(scope)) {
			syntaxError("Error during syntax analysis on rule StmtList");
			return false;
		}
		getAndCheckLexeme(false, {LexemType::rbrace});
		return true;
	} else if (_currentLexeme.type() == LexemType::kwreturn) {    // 2_26
		auto p = E(scope);
		if (!p) {
			syntaxError("Error during syntax analysis on rule E");
			return false;
		}
		generateAtoms(scope, std::make_shared<RetAtom>(p));
		getAndCheckLexeme(false, {LexemType::semicolon});
		return true;
	} else if (_currentLexeme.type() == LexemType::semicolon) {    // 2_27
		return true;
	}
	syntaxError("An undefined lexeme on rule StmtList appeared");
	return false;
}

bool Translator::AssignOrCallOp(Scope scope) {
	if (!AssignOrCall(scope)) { // 2_28
		syntaxError("Error during syntax analysis on rule AssignOrCall");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::semicolon});
	return true;
}

bool Translator::AssignOrCall(Scope scope) {
	auto q = _currentLexeme.str(); // 2_29
	if (!AssignOrCall_(scope, q)) {
		syntaxError("Error during syntax analysis on rule AssignOrCall_");
		return false;
	}
	return true;
}

bool Translator::AssignOrCall_(Scope scope, const std::string& p) {
	getAndCheckLexeme(false, {LexemType::opassign, LexemType::lpar});
	if (_currentLexeme.type() == LexemType::opassign) { // 2_30
		auto q = E(scope);
		if (!q) {
			syntaxError("Error during syntax analysis on rule E");
			return false;
		}
		auto r = checkVar(scope, p);
		generateAtoms(scope, std::make_shared<UnaryOpAtom>("MOV", q, r));
		return true;
	} else if (_currentLexeme.type() == LexemType::lpar) { // 2_31
		auto r = _symbolTable.alloc(scope);
		int n = ArgList(scope);
		if (n == -1) {
			syntaxError("Error while reading arguments in Arglist");
			return false;
		}
		auto q = checkFunc(p, n);
		if (!q) {
			syntaxError("Error on rule E1_; The number of function arguments does not match the definition");
			return false;
		}
		getAndCheckLexeme(false, {LexemType::rpar});
		generateAtoms(scope, std::make_shared<CallAtom>(q, r));
		return true;
	}

	syntaxError("An undefined lexeme on rule AssignOrCall_ appeared");
	return false;
}

bool Translator::WhileOp(Scope scope) { // 2_32
	auto l1 = newLabel();
	auto l2 = newLabel();
	generateAtoms(scope, std::make_shared<LabelAtom>(l1));
	getAndCheckLexeme(false, {LexemType::lpar});
	auto p = E(scope);
	if (!p) {
		syntaxError("Error on rule E1_; The number of function arguments does not match the definition");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::rpar});
	generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l2));
	if (!Stmt(scope)) {
		syntaxError("Error during syntax analysis on rule Stmt");
		return false;
	}
	generateAtoms(scope, std::make_shared<JumpAtom>(l1));
	generateAtoms(scope, std::make_shared<LabelAtom>(l2));
	return true;
}

bool Translator::ForOp(Scope scope) { // 2_33
	getAndCheckLexeme(false, {LexemType::lpar});
	auto l1 = newLabel();
	auto l2 = newLabel();
	auto l3 = newLabel();
	auto l4 = newLabel();
	if (!ForInit(scope)) {
		syntaxError("Error during syntax analysis on rule ForInit");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::semicolon});
	generateAtoms(scope, std::make_shared<LabelAtom>(l1));
	auto p = ForExp(scope);
	if (!p) {
		syntaxError("Error during syntax analysis on rule ForExp");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::semicolon});
	generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l4));
	generateAtoms(scope, std::make_shared<JumpAtom>(l3));
	generateAtoms(scope, std::make_shared<LabelAtom>(l2));
	if (!ForLoop(scope)) {
		syntaxError("Error during syntax analysis on rule ForLoop");
		return false;
	}
	generateAtoms(scope, std::make_shared<JumpAtom>(l1));
	getAndCheckLexeme(false, {LexemType::rpar});
	generateAtoms(scope, std::make_shared<LabelAtom>(l3));
	if (!Stmt(scope)) {
		syntaxError("Error during syntax analysis on rule Stmt");
		return false;
	}
	generateAtoms(scope, std::make_shared<JumpAtom>(l2));
	generateAtoms(scope, std::make_shared<LabelAtom>(l4));
	return true;
}

bool Translator::ForInit(Scope scope) {
	getAndCheckLexeme(scope);
	if (_currentLexeme.type() == LexemType::id) { // 2_34
		if (!AssignOrCall(scope)) {
			syntaxError("Error during syntax analysis on rule AssignOrCall");
			return false;
		}
		return true;
	}
	pushBackLexeme(); // 2_35
	return true;
}

std::shared_ptr<RValue> Translator::ForExp(Scope scope) {
	getAndCheckLexeme(false);
	if (_currentLexeme.type() == LexemType::opnot ||
	    _currentLexeme.type() == LexemType::lpar ||
	    _currentLexeme.type() == LexemType::num ||
	    _currentLexeme.type() == LexemType::chr ||
	    _currentLexeme.type() == LexemType::opinc ||
	    _currentLexeme.type() == LexemType::id) {  // 2_36
		pushBackLexeme();
		auto q = E(scope);
		if (!q) {
			syntaxError("Error during syntax analysis on rule E");
			return nullptr;
		}
		return q;
	}
	pushBackLexeme();   // 2_37
	return std::make_shared<NumberOperand>(1);
}

bool Translator::ForLoop(Scope scope) {
	getAndCheckLexeme(false);
	if (_currentLexeme.type() == LexemType::id) { // 2_38
		if (!AssignOrCall(scope)) {
			syntaxError("Error during syntax analysis on rule AssignOrCall");
			return false;
		}
		return true;
	} else if (_currentLexeme.type() == LexemType::opinc) { // 2_39
		getAndCheckLexeme(false, {LexemType::id});
		auto name = _currentLexeme.str();
		auto p = checkVar(scope, name);
		generateAtoms(scope, std::make_shared<BinaryOpAtom>("ADD", p, std::make_shared<NumberOperand>(1), p));
		return true;
	}
	pushBackLexeme(); // 2_40
	return true;
}

bool Translator::IfOp(Scope scope) {
	getAndCheckLexeme(false, {LexemType::lpar}); // 2_41
	auto l1 = newLabel();
	auto l2 = newLabel();
	auto p = E(scope);
	if (!p) {
		syntaxError("Error during syntax analysis on rule E");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::rpar});
	generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l1));
	if (!Stmt(scope)) {
		syntaxError("Error during syntax analysis on rule Stmt");
		return false;
	}
	generateAtoms(scope, std::make_shared<JumpAtom>(l2));
	generateAtoms(scope, std::make_shared<LabelAtom>(l1));
	if (!ElsePart(scope)) {
		syntaxError("Error during syntax analysis on rule ElsePart");
		return false;
	}
	generateAtoms(scope, std::make_shared<LabelAtom>(l2));
	return true;
}

bool Translator::ElsePart(Scope scope) {
	getAndCheckLexeme(false);
	if (_currentLexeme.type() == LexemType::kwelse) { // 2_42
		if (!Stmt(scope)) {
			syntaxError("Error during syntax analysis on rule Stmt");
			return false;
		}
		return true;
	}
	pushBackLexeme(); // 2_43
	return true;
}

bool Translator::SwitchOp(Scope scope) {
	getAndCheckLexeme(false, {LexemType::lpar}); // 2_44
	auto end = newLabel();
	auto p = E(scope);
	if (!p) {
		syntaxError("Error during syntax analysis on rule SwithcOp");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::rpar});
	getAndCheckLexeme(false, {LexemType::lbrace});
	if (!Cases(scope, p, end)) {
		syntaxError("Error during syntax analysis on rule Cases");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::rbrace});
	generateAtoms(scope, std::make_shared<LabelAtom>(end));
	return true;
}

bool Translator::Cases(Scope scope, const std::shared_ptr<RValue>& p, const std::shared_ptr<LabelOperand>& end) {
	auto& q = p;            // 2_45
	auto& end1 = end;
	auto def1 = ACase(scope, q, end1);
	if (!def1) {
		syntaxError("Error during syntax analysis on rule ACase");
		return false;
	}
	const auto& r = p;
	const auto& end2 = end;
	const auto& def2 = def1;
	if (!Cases_(scope, r, end2, def2)) {
		syntaxError("Error during syntax analysis on rule Cases_");
		return false;
	}
	return true;
}

bool Translator::Cases_(Scope scope, const std::shared_ptr<RValue>& p, const std::shared_ptr<LabelOperand>& end,
                        const std::shared_ptr<LabelOperand>& def) {
	getAndCheckLexeme(false);
	LabelOperand nullLabel(0);
	if (_currentLexeme.type() == LexemType::kwcase || _currentLexeme.type() == LexemType::kwdefault) { // 2_46
		// T: I hope, that kwcase lexem include `default`
		// 6W: well, it wasn't supposed. added recently
		pushBackLexeme();
		const auto& q = p;
		const auto& end1 = end;
		auto def1 = ACase(scope, q, end1);
		if (!def1) {
			syntaxError("Error during syntax analysis on rule ACase");
			return false;
		}
		std::shared_ptr<LabelOperand> def2;
		if (def != nullptr && def1 != nullptr && *def >= nullLabel && *def1 >= nullLabel) {
			syntaxError("Two default section");
			return false;
		} else def2 = def1 != nullptr && *def1 >= nullLabel ? def1 : def;
		const auto& r = p;
		const auto& end2 = end;
		if (!Cases_(scope, r, end2, def2)) {
			syntaxError("Error during syntax analysis on rule Cases_");
			return false;
		}
		return true;
	}
	pushBackLexeme();  // 2_47
	std::shared_ptr<LabelOperand> q;
	if (def != nullptr && *def >= nullLabel) q = def;
	else q = end;
	generateAtoms(scope, std::make_shared<JumpAtom>(q));
	return true;
}

std::shared_ptr<LabelOperand>
Translator::ACase(Scope scope, const std::shared_ptr<RValue>& p, const std::shared_ptr<LabelOperand>& end) {
	getAndCheckLexeme(false, {LexemType::kwcase, LexemType::kwdefault});
	if (_currentLexeme.type() == LexemType::kwcase) {  // 2_48
		getAndCheckLexeme(false, {LexemType::num});
		auto next = newLabel();
		generateAtoms(scope, std::make_shared<ConditionalJumpAtom>("NE", p, std::make_shared<NumberOperand>(
				_currentLexeme.value()), next));
		getAndCheckLexeme(false, {LexemType::colon});
		if (!Stmt(scope)) {
			syntaxError("Error during syntax analysis on rule Stmt");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<JumpAtom>(end));
		generateAtoms(scope, std::make_shared<LabelAtom>(next));
		return std::make_shared<LabelOperand>(-1);
	} else if (_currentLexeme.type() == LexemType::kwdefault) {  // 2_49
		getAndCheckLexeme(false, {LexemType::colon});
		auto next = newLabel();
		generateAtoms(scope, std::make_shared<JumpAtom>(next));
		auto def = newLabel();
		generateAtoms(scope, std::make_shared<LabelAtom>(def));
		if (!Stmt(scope)) {
			syntaxError("Error during syntax analysis on rule Stmt");
			return nullptr;
		}
		generateAtoms(scope, std::make_shared<JumpAtom>(end));
		generateAtoms(scope, std::make_shared<LabelAtom>(next));
		return def;
	}
	syntaxError("Undefined keyword in rule ACase");
	return nullptr;
}

bool Translator::IOp(Scope scope) {
	getAndCheckLexeme(false, {LexemType::id});        // 2_50
	auto name = _currentLexeme.str();
	getAndCheckLexeme(false, {LexemType::semicolon});
	auto p = checkVar(scope, name);
	generateAtoms(scope, std::make_shared<InAtom>(p));
	return true;
}

bool Translator::OOp(Scope scope) {
	if (!OOp_(scope)) {    // 2_51
		syntaxError("Error during syntax analysis on rule OOp_");
		return false;
	}
	getAndCheckLexeme(false, {LexemType::semicolon});
	return true;
}

bool Translator::OOp_(Scope scope) {
	getAndCheckLexeme(false);
	if (_currentLexeme.type() == LexemType::opnot ||
	    _currentLexeme.type() == LexemType::lpar ||
	    _currentLexeme.type() == LexemType::num ||
	    _currentLexeme.type() == LexemType::chr ||
	    _currentLexeme.type() == LexemType::opinc ||
	    _currentLexeme.type() == LexemType::id) {    // 2_52
		pushBackLexeme();
		auto p = E(scope);
		if (!p) {
			syntaxError("Error during syntax analysis on rule E");
			return false;
		}
		generateAtoms(scope, std::make_shared<OutAtom>(p));
		return true;
	} else if (_currentLexeme.type() == LexemType::str) {    // 2_53
		auto s = _currentLexeme.str();
		auto p = _stringTable.add(s);
		generateAtoms(scope, std::make_shared<OutAtom>(p));
		return true;
	}
	syntaxError("Undefined lexeme on rule OOp_");
	return false;
}

void Translator::startTranslation() {
	StmtList(GLOBAL_SCOPE);
	getAndCheckLexeme(true);
	if (_currentLexeme != LexemType::eof) {
		syntaxError("Syntax analysis was completed, but an additional lexeme appeared");
	}
	if (_symbolTable.checkFunc("main", 0) == nullptr) {
		syntaxError("A main function with 0 arguments expected, but it's not provided.");
	}
}

const SymbolTable& Translator::getSymbolTable() const {
	return _symbolTable;
}

const StringTable& Translator::getStringTable() const {
	return _stringTable;
}

std::string Translator::getLastLexemes() {
	std::string lexemesStr = "Last correctly read lexemes: ";
	std::deque<Token> st = _lastLexemes;
	if (st.size() < 3) {
		for (size_t i = 0; i < 3 - st.size(); i++) {
			lexemesStr += "[nothing]";
			if (i < 2) lexemesStr += ", ";
		}
		while (!st.empty()) {
			lexemesStr += "[" + st.front().toString() + "]";
			if (st.size() != 1) lexemesStr += ", ";
			st.pop_front();
		}
	} else {
		for (size_t i = 0; i < 3; i++) {
			lexemesStr += "[" + st.front().toString() + "]";
			if (i != 2) lexemesStr += ", ";
			st.pop_front();
		}
	}
	lexemesStr += "\n";
	return lexemesStr;
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

void Translator::generateProlog(std::ostream &stream) {
    stream << "ORG 0\n";
    stream << "LXI H, 0\n";
    stream << "SPHL\n";
    stream << "CALL main\n";
    stream << "END\n";
    stream << "@MULT:\n";
    stream << "; Code for MULT library function\n";
    stream << "@PRINT:\n";
    stream << "; Code for PRINT library function\n";
}

void Translator::generateFunction(std::ostream &stream, const std::pair<std::string, int>& par) {
    stream << "\n" + par.first + ":\n";
    stream << "LXI B, 0\n";
    auto m = _symbolTable.getM(par.second);
    for(int i = 0; i < m; i++) stream << "PUSH B\n";
    for(const auto& atom : _atoms[par.second]) {
        if(atom->toString()[1] == 'P') continue;
	    atom->generate(stream, this, par.second);
    }
}

void Translator::generateCode(std::ostream &stream) {
    stream << "ASM 8080 code:" << std::endl;
    for (size_t i = 0; i < 64; i++) stream << "-";
    stream << std::endl;
    _symbolTable.calculateOffset();
    stream << "ORG 8000H\n";
    _symbolTable.generateGlobals(stream);
    _stringTable.generateStrings(stream);
    generateProlog(stream);
    auto funcs = _symbolTable.functionNames();
    for(const auto& func : funcs) {
	    generateFunction(stream, func);
    }
}

TranslationException::TranslationException(std::string error) : _error(std::move(error)) {}

const char *TranslationException::what() const noexcept {
	return _error.c_str();
}

const char *CodeGenerationException::what() const noexcept {
	return _error.c_str();
}

CodeGenerationException::CodeGenerationException(std::string error) : _error(std::move(error)) {

}
