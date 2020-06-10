#include <iostream>
#include <sstream>
#include <fstream>
#include "Translator.h"

std::string getFullFilename(std::string string) {
	std::stringstream ss;
	for (auto it = string.rbegin(); it != string.rend(); ++it) {
		char c = *it;
		if (c == '/' || c == '\\') {
			break;
		}
		ss << c;
	}
	std::stringstream ss2;
	auto reversed = ss.str();
	for (auto it2 = reversed.rbegin(); it2 != reversed.rend(); ++it2) {
		ss2 << *it2;
	}
	return ss2.str();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-throw-by-value-catch-by-reference" // VS unexpected troubles

int main(int argc, char **argv) {
	std::ifstream ifile;
	std::string filename = "C:\\Users\\Throder\\source\\repos\\project-micric2\\test\\resources\\prog1.minic", input, output;
	int i = 1;
	std::string selfName = getFullFilename(argv[0]);
	if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
		std::cout << "Usage:" << std::endl
		          << '\t' << selfName << "(input prog.minic, output prog.minic.atom)" << std::endl
		          << '\t' << selfName << " [options] [target]" << std::endl
		          << '\t' << selfName << " [options] -i [target]" << std::endl
		          << "Options:" << std::endl
		          << '\t' << "-i file" << '\t' << "Set target file" << std::endl
		          << '\t' << "-o file" << '\t' << "Set output file" << std::endl;
		return 1;
	}
	while (i < argc) {
		input = std::string(argv[i]);
		if (input == "-i") {
			if (argc <= i + 1) {
				std::cerr << "Empty -i" << std::endl;
				return 1;
			}
			filename = argv[i + 1];
			i += 2;
		} else if (input == "-o") {
			if (argc <= i + 1) {
				std::cerr << "Empty -o" << std::endl;
				return 1;
			}
			output = argv[i + 1];
			i += 2;
		} else {
			if (!input.empty())
				filename = input;
			++i;
		}
	}
	ifile.open(filename);
	if (!ifile) {
		std::cerr << "Failed to open input file" << std::endl;
		return 1;
	}
	std::cout << "Opened input: " << filename << std::endl;
	std::ofstream ofile;
	if (output.empty()) {
		ofile.open(filename + ".atom");
	} else {
		ofile.open(output);
	}
	if (ofile) {
		std::cout << "Opened output: " << (output.empty() ? filename + ".atom" : output) << std::endl;
	}
	if (!ofile && !output.empty()) {
		std::cerr << "Failed to create output file" << std::endl;
		return 1;
	}
	Translator translator(ifile);
	try {
		translator.startTranslation();
		ifile.close();
		translator.printAtoms(ofile);
		ofile << std::endl;
		translator.printSymbolTable(ofile);
		ofile << std::endl;
		translator.printStringTable(ofile);
		ofile.close();
	} catch (TranslationException exception) {
		std::cerr << "Exception during compiling:" << std::endl;
		std::cerr << exception.what();
		ifile.close();
		ofile.close();
		return 2;
	}
}

#pragma clang diagnostic pop
