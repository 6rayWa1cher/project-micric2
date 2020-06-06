//
// Created by 6rayWa1cher and Throder on 04.06.2020.
//

#include "../include/Atoms.h"
#include "../include/StringTable.h"
#include "../include/SymbolTable.h"

Operand::Operand() = default;

RValue::RValue() = default;

MemoryOperand::MemoryOperand(unsigned int index, const SymbolTable * symbolTable) : _index(index),
_symbolTable(symbolTable) {

}

std::string MemoryOperand::toString() const {
	return std::to_string(this->_index) + "[" + this->_symbolTable->operator[](this->_index) + ']';
}

NumberOperand::NumberOperand(int value) : _value(value) {}

std::string NumberOperand::toString() const {
	return '`' + std::to_string(this->_value) + '`';
}

StringOperand::StringOperand(unsigned int index, const StringTable * stringTable) : _index(index),
_stringTable(stringTable) {}

std::string StringOperand::toString() const {
	return std::to_string(this->_index) + '{' + this->_stringTable->operator[](this->_index) + '}';
}

LabelOperand::LabelOperand(unsigned int labelId) : _labelId(labelId) {}

std::string LabelOperand::toString() const {
	return "L" + std::to_string(this->_labelId);
}

Atom::Atom() = default;

BinaryOpAtom::BinaryOpAtom(const std::string &name,
								 std::shared_ptr<RValue> left,
								 std::shared_ptr<RValue> right,
								 std::shared_ptr<MemoryOperand> result) : _name(name),
																		  _left(left),
																		  _right(right),
																		  _result(result) {}

std::string BinaryOpAtom::toString() const {
	return "(" + _name + ", " + _left->toString() + ", " +
								_right->toString() + ", " +
								_result->toString() + ")";
}

UnaryOpAtom::UnaryOpAtom(const std::string & name,
							   std::shared_ptr<RValue> operand,
							   std::shared_ptr<MemoryOperand> result) : _name(name),
																	    _operand(operand),
																	    _result(result) {}

std::string UnaryOpAtom::toString() const {
	return "(" + _name + ", " + _operand->toString() + ",, " +
								_result->toString() + ")";
}

OutAtom::OutAtom(std::shared_ptr<Operand> value) : _value(value) {}

std::string OutAtom::toString() const {
	return "(OUT,,, " + _value->toString() + ")";
}

InAtom::InAtom(std::shared_ptr<MemoryOperand> result) : _result(result) {}

std::string InAtom::toString() const {
	return "(IN,,, " + _result->toString() + ")";
}

LabelAtom::LabelAtom(std::shared_ptr<LabelOperand> label) : _label(label) {}

std::string LabelAtom::toString() const {
	return "(LBL,,, " + _label->toString() + ")";
}

JumpAtom::JumpAtom(std::shared_ptr<LabelOperand> label) : _label(label) {}

std::string JumpAtom::toString() const {
	return "(JMP,,, " + _label->toString() + ")";
}

ConditionalJumpAtom::ConditionalJumpAtom(const std::string & condition,
											   std::shared_ptr<RValue> left,
											   std::shared_ptr<RValue> right,
											   std::shared_ptr<LabelOperand> label) : _condition(condition),
																				 	  _left(left),
																					  _right(right),
																					  _label(label) {}

std::string ConditionalJumpAtom::toString() const {
	return "(" + _condition + ", " + _left->toString() + ", " +
									 _right->toString() + ", " +
									 _label->toString() + ")";
}

CallAtom::CallAtom(std::shared_ptr<MemoryOperand> function,
				   std::shared_ptr<MemoryOperand> result) : _function(function),
															_result(result) {}

std::string CallAtom::toString() const {
	return "(CALL, " + _function->toString() + ",, " + _result->toString() + ")";
}

RetAtom::RetAtom(std::shared_ptr<RValue> value) : _value(value) {}

std::string RetAtom::toString() const {
	return "(RET,,, " + _value->toString() + ")";
}

ParamAtom::ParamAtom(std::shared_ptr<RValue> value) : _value(value) {}

std::string ParamAtom::toString() const {
	return "(PARAM,,, " + _value->toString() + ")";
}