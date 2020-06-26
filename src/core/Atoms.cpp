//
// Created by 6rayWa1cher and Throder-TVRS on 04.06.2020.
//

#include "../include/Atoms.h"
#include "../include/Translator.h"
#include <iostream>

#include "../include/StringTable.h"
#include "../include/SymbolTable.h"
#include "../include/GlobalParameters.h"

Operand::Operand() = default;

RValue::RValue() = default;

MemoryOperand::MemoryOperand(size_t index, const SymbolTable *symbolTable) : _index(index),
                                                                             _symbolTable(symbolTable) {

}

std::string MemoryOperand::toString() const {
	if (GlobalParameters::getInstance().enableOperatorFormatter) {
		return std::to_string(this->_index) + "[" + this->_symbolTable->operator[](this->_index) + ']';
	} else {
		return std::to_string(this->_index);
	}
}

bool MemoryOperand::operator==(const MemoryOperand& rhs) const {
	return _index == rhs._index && _symbolTable == rhs._symbolTable;
}

bool MemoryOperand::operator!=(const MemoryOperand& rhs) const {
	return !(rhs == *this);
}

size_t MemoryOperand::index() const noexcept {
	return this->_index;
}

void MemoryOperand::load(std::ostream& stream) const {
	std::cout << _symbolTable << std::endl;
	std::cout << _symbolTable->_records.size() << std::endl;
	std::cout << this->_index << std::endl;
	std::cout << _symbolTable->_records[this->_index]._name << std::endl;
	if (_symbolTable->_records[this->_index]._scope == -1) {
		stream << "LDA var" + std::to_string(_symbolTable->_records[this->_index]._init) + "\n";
	} else {
		stream << "LXI H, " + std::to_string(_symbolTable->_records[this->_index]._offset) + "\n";
		stream << "DAD SP\n";
		stream << "MOV A, M\n";
	}
}

void MemoryOperand::save(std::ostream& stream) const {
	if (_symbolTable->_records[this->_index]._scope == -1) {
		stream << "STA var" + std::to_string(_symbolTable->_records[this->_index]._init) + "\n";
	} else {
		stream << "LXI H, " + std::to_string(_symbolTable->_records[this->_index]._offset) + "\n";
		stream << "DAD SP\n";
		stream << "MOV M, A\n";
	}
}

NumberOperand::NumberOperand(int value) : _value(value) {}

std::string NumberOperand::toString() const {
	return '`' + std::to_string(this->_value) + '`';
}

void NumberOperand::load(std::ostream& stream) const {
	stream << "MVI A, " + std::to_string(this->_value) + "\n";
}

StringOperand::StringOperand(size_t index, const StringTable *stringTable) : _index(index),
                                                                             _stringTable(stringTable) {}

std::string StringOperand::toString() const {
	if (GlobalParameters::getInstance().enableOperatorFormatter) {
		return "S" + std::to_string(this->_index) + '{' + this->_stringTable->operator[](this->_index) + '}';
	} else {
		return "S" + std::to_string(this->_index);
	}
}

LabelOperand::LabelOperand(int labelId) : _labelId(labelId) {}

std::string LabelOperand::toString() const {
	if (GlobalParameters::getInstance().enableOperatorFormatter) {
		return "L" + std::to_string(this->_labelId);
	} else {
		return std::to_string(this->_labelId);
	}
}

bool LabelOperand::operator>=(const LabelOperand& rhs) const {
	return this->_labelId >= rhs._labelId;
}

Atom::Atom() = default;

BinaryOpAtom::BinaryOpAtom(std::string name,
                           std::shared_ptr<RValue> left,
                           std::shared_ptr<RValue> right,
                           std::shared_ptr<MemoryOperand> result) : _name(std::move(name)),
                                                                    _left(std::move(left)),
                                                                    _right(std::move(right)),
                                                                    _result(std::move(result)) {}

std::string BinaryOpAtom::toString() const {
	return "(" + _name + ", " + _left->toString() + ", " +
	       _right->toString() + ", " +
	       _result->toString() + ")";
}

void BinaryOpAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " + toString() + "\n";
	_right->load(stream);
	if (_name == "MUL" || _name == "DIV") {
		stream << "MOV D, A\n";
	} else {
		stream << "MOV B, A\n";
	}
	_left->load(stream);
	if (_name == "MUL") {
		stream << "CALL @MULT\n";
		stream << "MOV A, C\n";
	} else if (_name == "DIV") {
		stream << "CALL @DIV\n";
		stream << "MOV A, C\n";
	} else {
		stream << _name + " B\n";
	}
	_result->save(stream);
}

UnaryOpAtom::UnaryOpAtom(std::string name,
                         std::shared_ptr<RValue> operand,
                         std::shared_ptr<MemoryOperand> result) : _name(std::move(name)),
                                                                  _operand(std::move(operand)),
                                                                  _result(std::move(result)) {}

std::string UnaryOpAtom::toString() const {
	return "(" + _name + ", " + _operand->toString() + ",, " +
	       _result->toString() + ")";
}

void UnaryOpAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " + toString() + "\n";
	if (_name == "MOV") {
		_operand->load(stream);
		_result->save(stream);
	} else if (_name == "NEG") {
		_operand->load(stream);
		stream << "CMA\n";
		stream << "INR A\n";
	} else if (_name == "NOT") {
		_operand->load(stream);
		stream << "CMA\n";
	} else {
		throw CodeGenerationException("Unexpected atom " + _name);
	}
}

OutAtom::OutAtom(std::shared_ptr<Operand> value) : _value(std::move(value)) {}

std::string OutAtom::toString() const {
	return "(OUT,,, " + _value->toString() + ")";
}

void OutAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " + toString() + "\n";
	if (_value->toString()[0] == 'S') {
		std::string buf;
		for (auto x : _value->toString()) if (x != 'S') buf += x;
		stream << "LXI A, str" + buf + "\n";
		stream << "CALL @print\n";
		return;
	}
	if (_value->toString()[0] == '\'') {
		std::dynamic_pointer_cast<NumberOperand>(_value)->load(stream);
	} else {
		std::dynamic_pointer_cast<MemoryOperand>(_value)->load(stream);
	}
	stream << "OUT 1\n";
}

InAtom::InAtom(std::shared_ptr<MemoryOperand> result) : _result(std::move(result)) {}

std::string InAtom::toString() const {
	return "(IN,,, " + _result->toString() + ")";
}

void InAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " << toString() + "\n";
	stream << "IN 0\n";
	_result->save(stream);
}

LabelAtom::LabelAtom(std::shared_ptr<LabelOperand> label) : _label(std::move(label)) {}

std::string LabelAtom::toString() const {
	return "(LBL,,, " + _label->toString() + ")";
}

void LabelAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " + toString() + "\n";
	stream << "LBL" + _label->toString() + ":\n";
}

JumpAtom::JumpAtom(std::shared_ptr<LabelOperand> label) : _label(std::move(label)) {}

std::string JumpAtom::toString() const {
	return "(JMP,,, " + _label->toString() + ")";
}

void JumpAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " + toString() + "\n";
	stream << "JMP LBL" + _label->toString() + "\n";
}

ConditionalJumpAtom::ConditionalJumpAtom(std::string condition,
                                         std::shared_ptr<RValue> left,
                                         std::shared_ptr<RValue> right,
                                         std::shared_ptr<LabelOperand> label) : _condition(std::move(condition)),
                                                                                _left(std::move(left)),
                                                                                _right(std::move(right)),
                                                                                _label(std::move(label)) {}

std::string ConditionalJumpAtom::toString() const {
	return "(" + _condition + ", " + _left->toString() + ", " +
	       _right->toString() + ", " +
	       _label->toString() + ")";
}

void
ConditionalJumpAtom::generate(std::ostream& stream, Translator *translator, int scope) const {        // TODO
	stream << "\t; " + toString() + "\n";
	_right->load(stream);
	stream << "MOV B, A\n";
	_left->load(stream);
	stream << "CMP B\n";
	if (_condition == "EQ") {
		stream << "JZ LBL" << _label->toString() << '\n';
	} else if (_condition == "NE") {
		stream << "JNZ LBL" << _label->toString() << '\n';
	} else if (_condition == "GT") {
		stream << "JP LBL" << _label->toString() << '\n';
	} else if (_condition == "LT") {
		stream << "JM LBL" << _label->toString() << '\n';
	} else if (_condition == "GE") {
		stream << "JM LBL" << _label->toString() << "A\n";
		stream << "JMP LBL" << _label->toString() << '\n';
		// yes, i know, that NOP isn't necessary. NOP looks too good :)
		stream << "LBL" << _label->toString() << "A: NOP";
	} else if (_condition == "LE") {
		stream << "JP LBL" << _label->toString() << "A\n";
		stream << "JMP LBL" << _label->toString() << '\n';
		stream << "LBL" << _label->toString() << "A: NOP";
	} else {
		throw CodeGenerationException("Unexpected condition " + _condition);
	}
}

CallAtom::CallAtom(std::shared_ptr<MemoryOperand> function,
                   std::shared_ptr<MemoryOperand> result) : _function(std::move(function)),
                                                            _result(std::move(result)) {}

std::string CallAtom::toString() const {
	return "(CALL, " + _function->toString() + ",, " + _result->toString() + ")";
}

void CallAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " + toString() + "\n";
	saveRegs(stream);
	stream << "LXI B, 0\n";
	stream << "PUSH B\n";
	const SymbolTable& table = translator->getSymbolTable();
	int n = table._records[_function->index()]._len;
	for (const auto& param : translator->codeGenFuncArgs) {
		stream << "LXI B, 0\n";
		param->load(stream);
		stream << "MOV C, A\n";
		stream << "PUSH B\n";
	}
	translator->codeGenFuncArgs.clear();
	stream << "CALL " + table._records[_function->index()]._name + "\n";
	for (int i = 0; i < n; i++) {
		stream << "POP B\n";
	}
	stream << "POP B\n";
	stream << "MOV A, B\n";
	_result->save(stream);
	loadRegs(stream);
}

void CallAtom::saveRegs(std::ostream& stream) {
	stream << "PUSH B\nPUSH D\n PUSH H\nPUSH PSW\n";
}

void CallAtom::loadRegs(std::ostream& stream) {
	stream << "POP PSW\nPOP H\nPOP D\nPOP B\n";
}

RetAtom::RetAtom(std::shared_ptr<RValue> value) : _value(std::move(value)) {}

std::string RetAtom::toString() const {
	return "(RET,,, " + _value->toString() + ")";
}

void RetAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	stream << "\t; " + toString() + "\n";
	const SymbolTable& table = translator->getSymbolTable();
	auto m = table.getM(scope);
	auto res = 2 * (m + table._records[scope]._len + 1);
	_value->load(stream);
	stream << "LXI H, " + std::to_string(res) + "\n";
	stream << "DAD SP\n";
	stream << "MOV M, A\n";
	for (int i = 0; i < m; i++) stream << "POP B\n";
	stream << "RET\n";
}

ParamAtom::ParamAtom(std::shared_ptr<RValue> value) : _value(std::move(value)) {}

std::string ParamAtom::toString() const {
	return "(PARAM,,, " + _value->toString() + ")";
}

void ParamAtom::generate(std::ostream& stream, Translator *translator, int scope) const {
	translator->codeGenFuncArgs.push_back(_value);
}




