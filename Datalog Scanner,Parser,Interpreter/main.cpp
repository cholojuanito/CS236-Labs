/*
*  Tanner Davis
*  main.cpp
*  Jan 14th 2018
*
*  Accepts a file name as a command line argument.
*  Creates an instance of the Scanner class and uses that to tokenize the file.
*/

//#include <ctime>
#include "Scanner.h"
#include "Parser.h"
#include "Interpreter.h"

int main(int argc, char* argv[]) {

	//clock_t begin = clock();

	if (argc > 0) {
		string fileName = string(argv[1]);
		ifstream file;
		file.open(fileName);

		if (!file.good()) {
			cout << "File '" << fileName << "' could not be opened." << endl;
			return 0;
		}
		// Scan and tokenize the file
		Scanner dataLogScanner = Scanner(&file, fileName);
		dataLogScanner.runScan();
		//dataLogScanner.printTokens();
		file.close();

		// Parse the tokens and check validity
		vector<Token*> tokens = dataLogScanner.getTokens();
		Parser dataLogParser = Parser(tokens);
		DatalogProgram* parsedProgram =  dataLogParser.createProgram();

		// Print results
		//cout << parsedProgram->toString();

		// Create a database of the schemes and facts and interpret the queries
		Interpreter interpreter = Interpreter(parsedProgram);
		interpreter.createDB();

		//cout << "ORIGINAL DATABASE:" << endl;
		//cout << interpreter.toString() << endl;

		interpreter.interpret();

		//cout << "END DATABASE:" << endl;
		//cout << interpreter.toString() << endl;

		delete parsedProgram;
	}
	else {
		cout << "No file name was given. Please try again with a file name." << endl;
	}

	//clock_t end = clock();
	//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//cout << "PROGRAM TOOK: " << elapsed_secs << " SECONDS." << endl;

	system("pause");
	return 0;
}
