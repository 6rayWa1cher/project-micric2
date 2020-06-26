//
// Created by 6rayWa1cher and Throder-TVRS on 04.06.2020.
//

#ifndef PROJECT_MICRIC2_ATOMS_H
#define PROJECT_MICRIC2_ATOMS_H

#include <string>
#include <memory>

class SymbolTable;

class StringTable;

class Translator;

class Operand {
public:
	Operand();

	virtual std::string toString() const = 0;
};

class RValue : public Operand {
public:
	RValue();

    virtual void load(std::ostream& stream) const = 0;
};

class MemoryOperand : public RValue {
protected:
	size_t _index;
    const SymbolTable *_symbolTable;

public:
    MemoryOperand(size_t index, const SymbolTable *symbolTable);

	std::string toString() const override;

	size_t index() const noexcept;

    void load(std::ostream& stream) const override;

    void save(std::ostream& stream) const;

	bool operator==(const MemoryOperand& rhs) const;

	bool operator!=(const MemoryOperand& rhs) const;
};

class NumberOperand : public RValue {
protected:
	int _value;
public:
	NumberOperand(int value);

	std::string toString() const override;

    void load(std::ostream& stream) const override;
};

class StringOperand : public Operand {
protected:
	size_t _index;
	const StringTable *_stringTable;
public:
	StringOperand(size_t index, const StringTable *stringTable);

	std::string toString() const override;
};

class LabelOperand : public Operand {
protected:
	int _labelId;
public:
	LabelOperand(int labelId);

	std::string toString() const override;

	bool operator>=(const LabelOperand& rhs) const;
};

class Atom {
public:
	Atom();

	virtual std::string toString() const = 0;

	virtual void generate(std::ostream& stream, Translator *translator, int scope) const = 0;
};

class BinaryOpAtom : public Atom {
protected:
	std::string _name;
	std::shared_ptr<RValue> _left;
	std::shared_ptr<RValue> _right;
	std::shared_ptr<MemoryOperand> _result;
public:
	BinaryOpAtom(std::string name,
	             std::shared_ptr<RValue> left,
	             std::shared_ptr<RValue> right,
	             std::shared_ptr<MemoryOperand> result);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};


class UnaryOpAtom : public Atom {
private:
	std::string _name;
	std::shared_ptr<RValue> _operand;
	std::shared_ptr<MemoryOperand> _result;
public:
	UnaryOpAtom(std::string name,
	            std::shared_ptr<RValue> operand,
	            std::shared_ptr<MemoryOperand> result);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

class OutAtom : public Atom {
private:
	std::shared_ptr<Operand> _value;
public:
	OutAtom(std::shared_ptr<Operand> value);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

class InAtom : public Atom {
private:
	std::shared_ptr<MemoryOperand> _result;
public:
	InAtom(std::shared_ptr<MemoryOperand> result);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

class LabelAtom : public Atom {
private:
	std::shared_ptr<LabelOperand> _label;
public:
	LabelAtom(std::shared_ptr<LabelOperand> label);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

class JumpAtom : public Atom {
private:
	std::shared_ptr<LabelOperand> _label;
public:
	JumpAtom(std::shared_ptr<LabelOperand> label);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

class ConditionalJumpAtom : public Atom {
protected:
	std::string _condition;
	std::shared_ptr<RValue> _left;
	std::shared_ptr<RValue> _right;
	std::shared_ptr<LabelOperand> _label;
public:
	ConditionalJumpAtom(std::string condition,
	                    std::shared_ptr<RValue> left,
	                    std::shared_ptr<RValue> right,
	                    std::shared_ptr<LabelOperand> label);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

class CallAtom : public Atom {
private:
	std::shared_ptr<MemoryOperand> _function;
	std::shared_ptr<MemoryOperand> _result;

    static void saveRegs(std::ostream& stream);

    static void loadRegs(std::ostream& stream);
public:
	CallAtom(std::shared_ptr<MemoryOperand> function, std::shared_ptr<MemoryOperand> result);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;

};

class RetAtom : public Atom {
private:
	std::shared_ptr<RValue> _value;
public:
	RetAtom(std::shared_ptr<RValue> value);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

class ParamAtom : public Atom {
private:
	std::shared_ptr<RValue> _value;
public:
	ParamAtom(std::shared_ptr<RValue> value);

	std::string toString() const override;

	void generate(std::ostream& stream, Translator *translator, int scope) const override;
};

#endif //PROJECT_MICRIC2_ATOMS_H
