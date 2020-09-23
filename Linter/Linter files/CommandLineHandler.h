#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
class CommandLineHandler {
	private:
		std::string inFileName;
		std::string outFileName;
		std::string errFileName;
	public:
		CommandLineHandler(int argc, char** argv) {
			parseCLIargs(argc, argv);
		}

		void parseCLIargs(int argc, char** argv) {
			//Parse command line arguments
			char * charFound;
			for (int i = 0; i < argc; i++) {
				charFound = strchr(argv[i], '=');
				if (charFound != nullptr) {
					//tokenize string on equals (LHS = key, RHS = value)
					char * key = strtok(argv[i], "=");
					char * value = strtok(NULL, "=");
					getFileInfo(key, value);
				}
			}
		}

		void getFileInfo(char * key, char * value) {
			//user submits input file
			if (strcmp(key, "input") == 0) {
				inFileName = value;
				std::cout << "Input file name: " << inFileName << std::endl;
			}
			//user submits output file
			else if(strcmp(key, "output") == 0) {
				outFileName = value;
				std::cout << "Output file name: " << outFileName << std::endl;
			}
			//user submits error file
			else if (strcmp(key, "error") == 0) {
				errFileName = value;
				std::cout << "Error file name: " << errFileName << std::endl;
			}
			else {
				std::cout << "bad command line argument: " << key << std::endl;
			}
		}

		//Make file names accessible to FileHandler
		std::vector<std::string> getFileNames() {
			std::vector<std::string> fileNames = { inFileName, outFileName, errFileName };
			return fileNames;
		}
		
};