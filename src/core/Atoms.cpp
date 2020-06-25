//
// Created by 6rayWa1cher and Throder-TVRS on 04.06.2020.
//

#include "../include/Atoms.h"

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
    if (_symbolTable->_records[this->_index]._scope == -1) {
        stream << "LDA var" + std::to_string(_symbolTable->_records[this->_index]._init) + "\n";
    }
    else {
        stream << "LXI H, " + std::to_string(_symbolTable->_records[this->_index]._offset) + "\n";
        stream << "DAD SP\n";
        stream << "MOV A, M\n";
    }
}

void MemoryOperand::save(std::ostream& stream) const {
    if(_symbolTable->_records[this->_index]._scope == -1) {
        stream << "STA var" + std::to_string(_symbolTable->_records[this->_index]._init) + "\n";
    }
    else {
        stream << "LXI H, " + std::to_string(_symbolTable->_records[this->_index]._offset) + "\n";
        stream << "DAD SP\n";
        stream << "MOV M, A\n";
    }
}

NumberOperand::NumberOperand(int value) : _value(value) {}

std::string NumberOperand::toString() const {
	return '`' + std::to_string(this->_value) + '`';
}

void NumberOperand::load(std::ostream &stream) const {
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

void BinaryOpAtom::generate(std::ostream &stream) const {     // TODO

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

void UnaryOpAtom::generate(std::ostream &stream) const {        // TODO

}

OutAtom::OutAtom(std::shared_ptr<Operand> value) : _value(std::move(value)) {}

std::string OutAtom::toString() const {
	return "(OUT,,, " + _value->toString() + ")";
}

void OutAtom::generate(std::ostream &stream) const {        // TODO

}

InAtom::InAtom(std::shared_ptr<MemoryOperand> result) : _result(std::move(result)) {}

std::string InAtom::toString() const {
	return "(IN,,, " + _result->toString() + ")";
}

void InAtom::generate(std::ostream &stream) const {         // TODO

}

LabelAtom::LabelAtom(std::shared_ptr<LabelOperand> label) : _label(std::move(label)) {}

std::string LabelAtom::toString() const {
	return "(LBL,,, " + _label->toString() + ")";
}

void LabelAtom::generate(std::ostream &stream) const {      // TODO

}

JumpAtom::JumpAtom(std::shared_ptr<LabelOperand> label) : _label(std::move(label)) {}

std::string JumpAtom::toString() const {
	return "(JMP,,, " + _label->toString() + ")";
}

void JumpAtom::generate(std::ostream &stream) const {       // TODO

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

void ConditionalJumpAtom::generate(std::ostream &stream) const {

}

CallAtom::CallAtom(std::shared_ptr<MemoryOperand> function,
                   std::shared_ptr<MemoryOperand> result) : _function(std::move(function)),
                                                            _result(std::move(result)) {}

std::string CallAtom::toString() const {
	return "(CALL, " + _function->toString() + ",, " + _result->toString() + ")";
}

void CallAtom::generateCondition(std::ostream &stream) const {      //TODO

}

RetAtom::RetAtom(std::shared_ptr<RValue> value) : _value(std::move(value)) {}

std::string RetAtom::toString() const {
	return "(RET,,, " + _value->toString() + ")";
}

void RetAtom::generateCondition(std::ostream &stream) const {       //TODO

}

ParamAtom::ParamAtom(std::shared_ptr<RValue> value) : _value(std::move(value)) {}

std::string ParamAtom::toString() const {
	return "(PARAM,,, " + _value->toString() + ")";
}

void ParamAtom::generateCondition(std::ostream &stream) const {     //TODO

}

void SimpleBinaryOpAtom::generateOperation(std::ostream &stream) const {    // ADD, SUB, AND, OR

}

void FnBinaryOpAtom::generateOperation(std::ostream &stream) const {        // MUL, DIV

}

void SimpleJumpAtom::generateCondition(std::ostream &stream) const {        // EQ, NE, GT, LT

}

void ComplexJumpAtom::generateCondition(std::ostream &stream) const {       // GE, LE

}
