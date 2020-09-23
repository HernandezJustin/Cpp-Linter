// Lint.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Command to compile with g++ : g++ Lint.cpp -o main
/* 
	USAGE: 
	1) with input file -> main input=test.cpp
	2) with output file -> main input=test.cpp output=testOut.cpp
	3) with error file -> main input=test.cpp error=error.txt (WIP)
*/ 

#include "pch.h"
#include "CommandLineHandler.h"
#include "FileHandler.h"
#include "Linter.h"
using namespace std;

int main(int argc, char ** argv)
{
	//CommandLineHandler * clh = new CommandLineHandler(argc, argv);
	//FileHandler * fh = new FileHandler(clh->getFileNames());
	//Linter * linter = new Linter(fh->validateFiles());

	CommandLineHandler * clh = nullptr;
	FileHandler * fh = nullptr;
	Linter * linter = nullptr;

	try {
		if ((clh = new CommandLineHandler(argc, argv)) != nullptr){}
		else {//error occurred
		}
			if ((fh = new FileHandler(clh->getFileNames())) != nullptr){}
			else {//error occurred
			}
				if ((linter = new Linter(fh->validateFiles())) != nullptr){}
				else{//error occurred
				}			
	}
	catch (exception e) {
		e.what();
	}

}

