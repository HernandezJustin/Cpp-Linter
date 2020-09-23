#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include "strCoords.h"
#include "ErrorHandler.h"
class Linter {
private:
	bool outputFilePresent = false;
	bool errFilePresent = false;
	bool lastToken = false;
	std::string inputFilePath;
	std::string outputFilePath;
	std::string lintedLine;
	std::ifstream file;
	std::ofstream outFile;
	int indentAmount, recursiveDepth = 0;
	int currentLineNumber;
	ErrorHandler * eHandler;
	enum class LineSectionCategory { QUOTE, COMMENT, LINTABLE, BLANK_LINE, PPD} lineSectionCategory;
	
public:
	Linter(std::map<std::string, int> files) {
		indentAmount = 3;
		currentLineNumber = 1;
		lintedLine = "";
		std::map<std::string, int>::iterator fileNameIterator;
		//Iterate through file names passed by FileHandler
		for (fileNameIterator = files.begin(); fileNameIterator != files.end(); fileNameIterator++)
		{	//found input file
			if(fileNameIterator->second == 0) inputFilePath = fileNameIterator->first;
			//found output file
			if (fileNameIterator->second == 1) {
				outputFilePresent = true;
				outputFilePath = fileNameIterator->first;
			}
			//found error file
			if (fileNameIterator->second == 2) errFilePresent = true;
		}
		//open and lint input file
		readFileContents(inputFilePath);
	}
	void readFileContents(std::string filePath) {
		std::string line;
		std::string formattedLine;
		//open input and file and output file, if present
		file.open(filePath);
		if (outputFilePresent) outFile.open(outputFilePath);
		if (file) {
			//parse input file line by line
			while (std::getline(file, line)) {
				//parse line to turn into tokens
				//std::cout << "Current Line Number" << currentLineNumber << std::endl;
				parseLine(line);
				if (outputFilePresent) {
					//write output to outFile
					outFile << lintedLine << '\n';
				}
				else {
					//write to stdout (console)
					std::cout << lintedLine << std::endl;
				}
				resetLintedLine();
				//reset lastToken to expect next token as start of next line
				//lastToken = false;
				currentLineNumber++;
			}
			file.close();
		}
		if (outputFilePresent) outFile.close();
	}

	void parseLine(std::string ln) {
		std::map<strCoords, LineSectionCategory> lintCoordinates = categorizeLineBySections(ln);
		tokenizeByCoordinates(lintCoordinates, ln);
	}

	std::map<strCoords, LineSectionCategory> categorizeLineBySections(std::string line) {
		std::map<strCoords, LineSectionCategory> lintCoordinates;
		strCoords coordinatePair;
		int quoteCount = 0;
		int quoteFirstPosition = 0;
		bool notInquote = true;
		bool inMultiLineComment = false;

		//Find the first character that is a quote or forward slash
		size_t charQuotePos = line.find_first_of("/\"*");

		//Find the first character that is NOT a quote or forward slash
		size_t notCharQuotePos = line.find_first_not_of("/\"*");

		while (charQuotePos != std::string::npos /*&& notCharQuotePos != std::string::npos*/) {
			if (notCharQuotePos < charQuotePos && notInquote && !inMultiLineComment) {
				if (line[charQuotePos + 1] == '/' || line[charQuotePos] == '\"') {
					coordinatePair.x = notCharQuotePos;
					coordinatePair.y = charQuotePos - 1;
					lintCoordinates.emplace(coordinatePair, LineSectionCategory::LINTABLE);
					notCharQuotePos = line.find_first_not_of("/\"*", charQuotePos);
				} 
				else {
					charQuotePos = line.find_first_of("/\"*", charQuotePos + 1);
				}
			}
			else {
				if (line[charQuotePos] == '/') {
					if (line[charQuotePos + 1] == '/') {
						//comment found: mark the found position of the comment until EOL
						coordinatePair.x = charQuotePos;
						coordinatePair.y = line.length() - 1;
						lintCoordinates.emplace(coordinatePair, LineSectionCategory::COMMENT);
						break;
					}
					else if (line[charQuotePos + 1] == '*') {
						//mutli-line comment found: mark the found position of the comment until EOL and trigger multi-line flag
						coordinatePair.x = charQuotePos;
						coordinatePair.y = line.length() - 1;
						lintCoordinates.emplace(coordinatePair, LineSectionCategory::COMMENT);
						inMultiLineComment = true;
						break;
					}	
				}
				else if (line[charQuotePos] == '*' && line[charQuotePos + 1] == '/') {
					//termination of multi-line comment
					coordinatePair.x = charQuotePos;
					coordinatePair.y = line.length() - 1;
					lintCoordinates.emplace(coordinatePair, LineSectionCategory::COMMENT);
					inMultiLineComment = false;
				}
				else {
					//quote found
					quoteCount++;
					notInquote = !notInquote;
					
					if (quoteCount % 2 == 0) {
						coordinatePair.x = quoteFirstPosition;
						coordinatePair.y = charQuotePos;
						lintCoordinates.emplace(coordinatePair , LineSectionCategory::QUOTE);

						if (charQuotePos != line.length() - 1) notCharQuotePos = charQuotePos + 1;
						else break;

						charQuotePos = line.find_first_of("/\"*", charQuotePos + 1);

						if (charQuotePos == std::string::npos && notCharQuotePos != std::string::npos) {
							coordinatePair.x = notCharQuotePos;
							coordinatePair.y = line.length() - 1;
							lintCoordinates.emplace(coordinatePair, LineSectionCategory::LINTABLE);
						}
					}
					else {
						quoteFirstPosition = charQuotePos;
						charQuotePos = line.find("\"", charQuotePos + 1);
					}
				}
			}
		}

		if (lintCoordinates.empty()) {
			if (line.length() == 0) {
				//blank line
				coordinatePair.x = 0;
				coordinatePair.y = 0;
				lintCoordinates.emplace(coordinatePair, LineSectionCategory::BLANK_LINE);
			}
			else {
				//not a blank line
				coordinatePair.x = 0;
				coordinatePair.y = line.length() - 1;
				if (line.find("#include") != std::string::npos) {
					//found preprocessor directive
					lintCoordinates.emplace(coordinatePair, LineSectionCategory::PPD);
				}
				else {
					lintCoordinates.emplace(coordinatePair, LineSectionCategory::LINTABLE);
				}
			}
		} 

		//debugCoordinates(lintCoordinates);
		return lintCoordinates;
	}

	void tokenizeByCoordinates(std::map<strCoords, LineSectionCategory> lintCoordinates, std::string line) {
		int substrLen;

		//Check if first map entry y-val is EOL
		//if (lintCoordinates.begin()->first.x == 0 && lintCoordinates.begin()->first.y == line.length() - 1) addToLintedLine(line);

		for (auto rit = lintCoordinates.crbegin(); rit != lintCoordinates.crend(); ++rit) {
			substrLen = (rit->first.y - rit->first.x) + 1;
			lineSectionCategory = rit->second;
			//std::cout << "Type of lsc: " << lineSectionCategoryToStr(lineSectionCategory) << " | Substr len: " << substrLen << " | Coords: " << "(" << rit->first.x << ", " << rit->first.y << ")\n";
			switch (lineSectionCategory) {
			case LineSectionCategory::BLANK_LINE:
				break;
			case LineSectionCategory::LINTABLE:
				//std::cout << "Line with linted ops: " << lintNextTokenSpace(line.substr(coordinatesIterator->first.x, substrLen)) << std::endl;
				prependToLintedLine(lintNextTokenSpace(line.substr(rit->first.x, substrLen)));
				break;
			case LineSectionCategory::QUOTE:
				prependToLintedLine(line.substr(rit->first.x, substrLen));
				break;
			case LineSectionCategory::COMMENT:
				prependToLintedLine(line.substr(rit->first.x, substrLen));
				break;
			case LineSectionCategory::PPD:
				prependToLintedLine(line.substr(rit->first.x, substrLen));
				break;
			default:
				//error occurred: LineSectionCategory is undefined here
				break;
			}
		}
	}

	std::string lintNextTokenSpace(std::string token) {
		const int SIZE = 11;
		std::string delims[SIZE] = { "<<", ">>", "==", "&&", "||", "+=", "-=", "*=", "/=", ">=", "<=" };
		for (int i = 0; i < SIZE; i++) {
			size_t delimPos = token.find(delims[i]);
			while (delimPos != std::string::npos) {
				//std::cout << "Found a :" << delims[i] << " at position: " << delimPos << " , on line #" << currentLineNumber << std::endl;
				lintSpacesNearOperator(&token, delimPos, 2);
				delimPos = token.find(delims[i], delimPos + 2);
			}
		}
		for (int i = 0; i < SIZE; i++) {
			delims[i].pop_back();
			size_t delimPos = token.find(delims[i]);
			while (delimPos != std::string::npos) {
				if (token.at(delimPos + 1) == delims[i][0]) {
					delimPos = token.find(delims[i], (delimPos + 2));
				}
				else {
					lintSpacesNearOperator(&token, delimPos, 1);
					delimPos = token.find(delims[i], (delimPos + 1));
				}
			}
		}
		return token;
	}

	void lintSpacesNearOperator(std::string* token, int delimPos, int opLength) {
		if (opLength == token->length()) token->insert(opLength, " ").insert(0, " ");
		else if (token->at(delimPos + 1) != ' ') {
			token->insert(delimPos + opLength, " ");
			if(token->at(delimPos - 1) != ' ') token->insert(delimPos, " ");
		}
	}

	void addToLintedLine(std::string token) {
		lintedLine.append(token);
	}

	void prependToLintedLine(std::string token) {
		lintedLine.insert(0, token);
	}

	std::string getLintedLine() {
		return lintedLine;
	}

	void resetLintedLine() {
		lintedLine = "";
	}
	void addIndentDepth() {
		indentAmount += 3;
	}

	void resetIndentDepth() {
		indentAmount = 3;
	}

	std::string indentToken(std::string token) {
		return token.insert(0, indentAmount, ' ');
	}

	//debug functions

	void debugCoordinates(std::map<strCoords, LineSectionCategory> lintCoordinates) {
		for (auto entry : lintCoordinates) {
			if (entry.second == LineSectionCategory::QUOTE) {
				std::cout << "QUOTE from: (" << entry.first.x << "," << entry.first.y << ")" << std::endl;
			}
			else if (entry.second == LineSectionCategory::COMMENT) {
				std::cout << "COMMENT from: (" << entry.first.x << "," << entry.first.y << ")" << std::endl;
			}
			else if (entry.second == LineSectionCategory::BLANK_LINE) {
				std::cout << "BLANK LINE" << std::endl;
			}
			else if (entry.second == LineSectionCategory::PPD) {
				std::cout << "PPD" << std::endl;
			}
			else {
				std::cout << "LINTABLE from: (" << entry.first.x << "," << entry.first.y << ")" << std::endl;
			}
		}
		std::cout << std::endl;
	}

	std::string lineSectionCategoryToStr(LineSectionCategory lsc) {
		switch (lsc)
		{
		case Linter::LineSectionCategory::QUOTE:
			return "QUOTE";
			break;
		case Linter::LineSectionCategory::COMMENT:
			return "COMMENT";
			break;
		case Linter::LineSectionCategory::LINTABLE:
			return "LINTABLE";
			break;
		case Linter::LineSectionCategory::BLANK_LINE:
			return "BLANK_LINE";
			break;
		case Linter::LineSectionCategory::PPD:
			return "PPD";
			break;
		default:
			return "ERROR";
			break;
		}
	}
};