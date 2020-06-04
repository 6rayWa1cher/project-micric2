//
// Created by 6rayWa1cher on 04.06.2020.
//

#ifndef PROJECT_MICRIC2_ATOMS_H
#define PROJECT_MICRIC2_ATOMS_H

#include <string>

class SymbolTable;

class StringTable;

class Operand {
public:
	Operand();

	virtual std::string toString() const = 0;
};

class RValue : Operand {
public:
	RValue();
};

class MemoryOperand : RValue {
protected:
	unsigned int _index;
	const SymbolTable *_symbolTable;
public:
	MemoryOperand(unsigned int index, const SymbolTable *symbolTable);

	std::string toString() const override;
};

class NumberOperand : RValue {
protected:
	int _value;
public:
	NumberOperand(int value);

	std::string toString() const override;
};

class StringOperand : Operand {
protected:
	unsigned int _index;
	const StringTable *_stringTable;
public:
	StringOperand(unsigned int index, const StringTable *stringTable);

	std::string toString() const override;
};

class LabelOperand : Operand {
protected:
	unsigned int _labelId;
public:
	LabelOperand(unsigned int labelId);

	std::string toString() const override;
};

#endif //PROJECT_MICRIC2_ATOMS_H
