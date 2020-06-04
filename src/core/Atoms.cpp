//
// Created by 6rayWa1cher on 04.06.2020.
//

#include "../include/Atoms.h"
#include "../include/StringTable.h"
#include "../include/SymbolTable.h"

Operand::Operand() = default;

RValue::RValue() = default;

MemoryOperand::MemoryOperand(unsigned int index, const SymbolTable *symbolTable) : _index(index),
                                                                                   _symbolTable(symbolTable) {

}

std::string MemoryOperand::toString() const {
	return std::to_string(this->_index) + "[" + this->_symbolTable->operator[](this->_index) + ']';
}

NumberOperand::NumberOperand(int value) : _value(value) {}

std::string NumberOperand::toString() const {
	return '`' + std::to_string(this->_value) + '`';
}

StringOperand::StringOperand(unsigned int index, const StringTable *stringTable) : _index(index),
                                                                                   _stringTable(stringTable) {}

std::string StringOperand::toString() const {
	return std::to_string(this->_index) + '{' + this->_stringTable->operator[](this->_index) + '}';
}

LabelOperand::LabelOperand(unsigned int labelId) : _labelId(labelId) {}

std::string LabelOperand::toString() const {
	return std::to_string(this->_labelId);
}
