//
// Created by 6rayWa1cher on 08.06.2020.
//

#include "tools.h"
#include "../src/include/GlobalParameters.h"

std::vector<std::string> split(const std::string& string, char delimiter) {
	std::vector<std::string> out;
	std::stringstream sb;
	for (char c : string) {
		if (c == delimiter) {
			out.push_back(sb.str());
			sb = std::stringstream();
		} else {
			sb << c;
		}
	}
	out.push_back(sb.str());
	return out;
}

std::vector<std::string>
getAtomsExpression(const std::string& s, std::vector<std::string> vars, const Scope expectedScope) {
	GlobalParameters::getInstance().enableOperatorFormatter = true;
	class LocalTranslator : public Translator {
	private:
		Scope _scope;
	public:
		explicit LocalTranslator(std::vector<std::string>& vars, Scope scope, std::istream& inputStream) : Translator(
				inputStream), _scope(scope) {
			for (const std::string& var : vars) {
				_symbolTable.addVar(var, scope, SymbolTable::TableRecord::RecordType::integer);
			}
		};

		void startTranslation() override {
			E(_scope);
			getAndCheckLexeme(true);
			if (_currentLexeme != LexemType::eof) {
				pushBackLexeme();
				syntaxError("Syntax analysis was completed, but an additional lexeme appeared");
			}
		}
	};
	auto iss = std::istringstream(s);
	LocalTranslator translator(vars, expectedScope, iss);
	translator.startTranslation();
	std::ostringstream oss;
	translator.printAtoms(oss);
	std::string out = oss.str();
	std::vector<std::string> scopeAtomVector = split(out, '\n');
	scopeAtomVector.erase(scopeAtomVector.end() - 1);
	std::vector<std::string> outVector;
	for (const std::string& scopeAtom : scopeAtomVector) {
		auto spliited = split(scopeAtom, '\t');
		Scope actualScope = std::stoi(spliited[0]);
		if (actualScope != expectedScope) {
			throw std::exception();
		}
		outVector.push_back(spliited[1]);
	}
	return outVector;
}