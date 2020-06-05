#include "../include/Translator.h"
#include "../include/Atoms.h"
#include "../include/StringTable.h"
#include "../include/SymbolTable.h"


Translator::Translator(std::istream& inputStream) : _scanner(Scanner(inputStream)) {
	labelCount = 0;
}

void Translator::printAtoms(std::ostream& stream) {
	for (const auto& atom : _atoms) {
		stream << atom->toString() << std::endl;
	}
}

void Translator::generateAtoms(std::shared_ptr<Atom> atom) {
	_atoms.push_back(atom);
}

std::shared_ptr<LabelOperand> Translator::newLabel() {
	auto label = std::make_shared<LabelOperand>(labelCount);
	labelCount++;
	return label;
}

void Translator::syntaxError(const std::string& message) {
	throw TranslatorException(message);
}

void Translator::lexicalError(const std::string& message) {
	throw TranslatorException(message);
}

TranslatorException::TranslatorException(std::string error) : _error(error) {}

const char* TranslatorException::what() const noexcept{
	return _error.c_str();
}