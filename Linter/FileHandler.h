#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
class FileHandler {
private:
	std::ifstream file;
	std::vector<std::string> fileNames;
	std::string relPaths[3] = { "./inFiles/" , "./outFiles/" , "./errFiles/" };
	const std::string VALID_FILE_EXTNS[5] = { ".txt", ".cpp", ".hpp", ".c", ".h" };
public: 
	FileHandler(std::vector<std::string> files) {
		setFileNames(files);
		//fileNames = files;
	}

	//Pass the valid files to the linter
	std::map<std::string, int> validateFiles() {
		//0 - Input File, 1 - Output File, 2 - Error File
		int counter = 0;
		std::map<std::string, int> mymap;
		for (auto name : fileNames) {
			if (isFileValid(name, counter)) {
				mymap.emplace(relPaths[counter], counter);
			}
			counter++;
		}
		return mymap;
	}

	bool isFileValid(std::string fileName, int fileType) {
		bool fileValidity = false;
		//Check if the file name is an empty string
		if (fileName.compare("") == 0) return false;
		//Check if the file has a '.' to separate the extension from file name
		std::size_t foundPos = fileName.find(".");
		//If no '.' is present OR the '.' is the leading character in the string (no file name) return
		if (foundPos == std::string::npos || foundPos == 0) return false;
		//Take a substring of the file name to obtain an extension
		std::string fileNameExtn = fileName.substr(foundPos, fileName.length());

		//Step 1: Ensure that the submitted file extension is valid
		//Step 2: Append file name to path and try to open the file
		switch (fileType) {
		case 0: case 1:
			for (int i = 0; i < VALID_FILE_EXTNS->length() + 1; i++) {
				if (VALID_FILE_EXTNS[i].compare(fileNameExtn) == 0) break;
				if (i == VALID_FILE_EXTNS->length()) return false;
			}
			break;
		case 2:
			if (fileNameExtn.compare(".txt") != 0) return false;
			break;
		default:
			std::cout << "Error occurred, unexpected input";
			return false;
			break;
		}
		relPaths[fileType] += fileName;
		fileValidity = fileCanOpen(relPaths[fileType]);
		
		return fileValidity;
	}

	bool fileCanOpen(std::string filePath) {
		file.open(filePath);
		if (!file) {
			//File not present or corrupted
			std::cout << "Error reading file";
			return false;
		}
		file.close();
		//File opened and closed succesfully
		return true;
	}

	void setFileNames(std::vector<std::string> fileNames) {
		this->fileNames = fileNames;
	}

};