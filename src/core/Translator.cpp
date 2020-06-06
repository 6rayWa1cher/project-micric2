//
// Created by Throder and 6rayWa1cher on 05.06.2020.
//

#include "../include/Translator.h"
#include "../include/Atoms.h"
#include "../include/StringTable.h"
#include "../include/SymbolTable.h"


Translator::Translator(std::istream& inputStream) : _scanner(Scanner(inputStream)) {
	_labelCount = 0;
}

void Translator::printAtoms(std::ostream& stream) {
	for (const auto& atom : _atoms) {
		stream << atom->toString() << std::endl;
	}
}

void Translator::printSymbolTable(std::ostream& stream) {
	_symbolTable.printSymbolTable(stream);
}

void Translator::generateAtoms(std::shared_ptr<Atom> atom) {
	_atoms.push_back(atom);
}

std::shared_ptr<LabelOperand> Translator::newLabel() {
	auto label = std::make_shared<LabelOperand>(_labelCount);
	_labelCount++;
	return label;
}

void Translator::syntaxError(const std::string& message) {
	throw TranslationException(message);
}

void Translator::lexicalError(const std::string& message) {
	throw TranslationException(message);
}

void Translator::getAndCheckLexem(bool eofAcceptable) {
	_currentLexem = _scanner.getNextToken();
	_lastLexems.push_back(_currentLexem);
	if (_lastLexems.size() == 4) _lastLexems.pop_front();
	while (_lastLexems.back().type() != LexemType::eof && _lastLexems.size() > 3) {
		_lastLexems.pop_front();
	}
	if (_currentLexem.type() == LexemType::error) {
		lexicalError(_currentLexem.toString());
	}
	if (!eofAcceptable && _currentLexem.type() == LexemType::eof) {
		syntaxError("Reached EOF before end of a syntax analysis");
	}
}

void Translator::pushBackLexem() {
	_scanner.pushBack(_currentLexem);
	if (_lastLexems.back() == _currentLexem) {
		_lastLexems.pop_back();
	}
}


std::shared_ptr<RValue> Translator::E() {
	auto q = E7();
	if (!q) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	return q;
}

std::shared_ptr<RValue> Translator::E7() {
	// 2
	auto q = E6();
	if (!q) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	auto s = E7_(q);
	if (!s) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E7_(std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opor) { // 3
		auto r = E6();
		if (!r) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		auto s = _symbolTable.alloc();
		generateAtoms(std::make_shared<BinaryOpAtom>("OR", p, r, s));
		auto t = E7_(s);
		if (!t) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		return t;
	} else { // 4
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E6() {
	// 5
	auto q = E5();
	if (!q) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	auto s = E6_(q);
	if (!s) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E6_(std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opand) { // 6
		auto r = E5();
		if (!r) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		auto s = _symbolTable.alloc();
		generateAtoms(std::make_shared<BinaryOpAtom>("AND", p, r, s));
		auto t = E6_(s);
		if (!t) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		return t;
	} else { // 7
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E5() {
	// 8
	auto q = E4();
	if (!q) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	auto s = E5_(q);
	if (!s) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E5_(std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (
			_currentLexem.type() == LexemType::opeq ||
			_currentLexem.type() == LexemType::opne ||
			_currentLexem.type() == LexemType::opgt ||
			_currentLexem.type() == LexemType::oplt ||
			_currentLexem.type() == LexemType::ople
			) { // 9-13
		auto savedLexem = _currentLexem;
		auto r = E4();
		if (!r) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		auto s = _symbolTable.alloc();
		auto l = this->newLabel();
		generateAtoms(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s));
		switch (savedLexem.type()) {
			case LexemType::opeq:
				generateAtoms(std::make_shared<ConditionalJumpAtom>("EQ", p, r, l));
				break;
			case LexemType::opne:
				generateAtoms(std::make_shared<ConditionalJumpAtom>("NE", p, r, l));
				break;
			case LexemType::opgt:
				generateAtoms(std::make_shared<ConditionalJumpAtom>("GT", p, r, l));
				break;
			case LexemType::oplt:
				generateAtoms(std::make_shared<ConditionalJumpAtom>("LT", p, r, l));
				break;
			case LexemType::ople:
				generateAtoms(std::make_shared<ConditionalJumpAtom>("LE", p, r, l));
				break;
			default:
				syntaxError("erm... wat?");
		}
		generateAtoms(std::make_shared<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s));
		generateAtoms(std::make_shared<LabelAtom>(l));
		return s;
	} else { // 14
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E4() {
	// 15
	auto q = E3();
	if (!q) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	auto s = E4_(q);
	if (!s) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E4_(std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opplus) { // 16
		auto r = E3();
		if (!r) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		auto s = _symbolTable.alloc();
		generateAtoms(std::make_shared<BinaryOpAtom>("ADD", p, r, s));
		auto t = E4_(s);
		if (!t) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		return t;
	} else if (_currentLexem.type() == LexemType::opminus) { // 17
		auto r = E3();
		if (!r) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		auto s = _symbolTable.alloc();
		generateAtoms(std::make_shared<BinaryOpAtom>("SUB", p, r, s));
		auto t = E4_(s);
		if (!t) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		return t;
	} else {// 18
		pushBackLexem();
		return p;
	}
}


std::shared_ptr<RValue> Translator::E3() {
	// 19
	auto q = E2();
	if (!q) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	auto s = E3_(q);
	if (!s) {
		syntaxError("Error during syntax analysis");
		return nullptr;
	}
	return s;
}

std::shared_ptr<RValue> Translator::E3_(std::shared_ptr<RValue> p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opmult) { // 20
		auto r = E2();
		if (!r) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		auto s = _symbolTable.alloc();
		generateAtoms(std::make_shared<BinaryOpAtom>("MUL", p, r, s));
		auto t = E3_(s);
		return t;
	} else { // 21
		pushBackLexem();
		return p;
	}
}

std::shared_ptr<RValue> Translator::E2() {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opnot) { // 22
		auto q = E1();
		if (!q) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		auto r = _symbolTable.alloc();
		generateAtoms(std::make_shared<UnaryOpAtom>("NOT", q, r));
		return r;
	} else { // 23
		pushBackLexem();
		return E1();
	}
}

std::shared_ptr<RValue> Translator::E1() {
	getAndCheckLexem();
	if (_currentLexem.type() == LexemType::lpar) { // 24
		auto q = this->E();
		if (!q) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		getAndCheckLexem();
		if (_currentLexem.type() != LexemType::rpar) {
			syntaxError("Not matching lexem type: expected lpar; got " + _currentLexem.toString());
			return nullptr;
		}
		return q;
	}
	if (_currentLexem.type() == LexemType::num) { // 25
		return std::make_shared<NumberOperand>(_currentLexem.value());
	}
	if (_currentLexem.type() == LexemType::chr) { // 26
		return std::make_shared<NumberOperand>(_currentLexem.value());
	}
	if (_currentLexem.type() == LexemType::opinc) { // 27
		getAndCheckLexem();
		if (_currentLexem.type() == LexemType::id) {
			auto name = _currentLexem.str();
			auto q = _symbolTable.add(name);
			generateAtoms(std::make_shared<BinaryOpAtom>("ADD", q, std::make_shared<NumberOperand>(1), q));
			return q;
		}
		syntaxError("Not matching lexem type: expected id; got " + _currentLexem.toString());
	}
	if (_currentLexem.type() == LexemType::id) {
		auto name = _currentLexem.str();
		auto s = E1_(name);
		if (!s) {
			syntaxError("Error during syntax analysis");
			return nullptr;
		}
		return s;
	}
	syntaxError("Not matching lexem type: expected num, chr, id or ++; got " + _currentLexem.toString());
	return nullptr;
}


std::shared_ptr<RValue> Translator::E1_(const std::string& p) {
	getAndCheckLexem(true);
	if (_currentLexem.type() == LexemType::opinc) { // 29
		auto s = _symbolTable.add(p);
		auto r = _symbolTable.alloc();
		generateAtoms(std::make_shared<UnaryOpAtom>("MOV", s, r));
		generateAtoms(std::make_shared<BinaryOpAtom>("ADD", s, std::make_shared<NumberOperand>(1), s));
		return r;
	}
	// TODO: 30
	pushBackLexem();
	return _symbolTable.add(p);
}

void Translator::startTranslation() {
	E();
	const Token& token = _scanner.getNextToken();
	if (token != LexemType::eof) {
		syntaxError("Syntax analysis ended, but additional token appears");
	}
}

const SymbolTable& Translator::getSymbolTable() const {
	return _symbolTable;
}

const StringTable& Translator::getStringTable() const {
	return _stringTable;
}

const std::deque<Token>& Translator::getLastLexems() const {
	return _lastLexems;
}


TranslationException::TranslationException(std::string error) : _error(error) {}

const char *TranslationException::what() const noexcept {
	return _error.c_str();
}
