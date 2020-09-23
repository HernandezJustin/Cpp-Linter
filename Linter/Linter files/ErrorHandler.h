#pragma once
#include <string>
#include <map>
class ErrorHandler {
public:
	enum ErrorType { SEMI_MISSING, PAREN_MISMATCH, BRACE_MISMATCH, BRACKET_MISMATCH, QUOTE_MISMATCH } errorTypes;
private:
	std::string line;
	std::string token;
	int errAmount;
	bool errFilePresent;
	std::map<int, ErrorType> errors;

public:
	ErrorHandler(bool errFilePresent) {
		this->errFilePresent = errFilePresent;
		errAmount = 0;
	}

	bool errorOccured(std::string line) {
		return true;
	}

	void addError(int lineNum, ErrorType eType) {
		errAmount++;
		errors.emplace(lineNum, eType);
	}

	ErrorType getErrorTypes() {
		return errorTypes;
	}
};