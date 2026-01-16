/* preprocessor.cpp */

#include "preprocessor.hpp"
#include <algorithm>

const std::vector<std::string> keywords = {
	"else",
	"macro",
	"ifdef",
	"import",
	"elifdef",
	"directive",
};

std::vector<Macro> macros;
std::vector<std::string> constant_expressions;

std::string Preprocessor::read_preprocessor_keyword(File &file) {
	std::string keyword;
	int c = file.get();

	while (!file.iseof() && isalpha(c)) {
		keyword += c;
		c = file.get();
	}

	file.unget();
	return keyword;
}

bool Preprocessor::is_preprocessor_keyword(const std::string &s) {
	return std::find(keywords.begin(), keywords.end(), s) != keywords.end();
}

bool Preprocessor::read_import(File &file) {
	std::string word;
	int c;

	while ((c = file.get()) != EOF) {
		if (isalpha(c)) {
			word = read_preprocessor_keyword(file);
			if (is_preprocessor_keyword(word)) return true;
		}
	}

	return false;
}

void Preprocessor::dependencies_analysis(File &file, const DiagnosticEngine &diagnostic_engine) {
	while (true) {
		if (read_import(file)) {
			
		}
	}
}

int Preprocessor::analyze(File &file, const DiagnosticEngine &diagnostic_engine) {
	return SIGNAL_END_ANALYSIS;
}