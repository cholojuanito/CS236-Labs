#pragma once
/*
*  Tanner Davis
*  Scanner.h
*  Jan 14th 2018
*
*  Scanner class:
*  Scans a Datalog file and tokenizes the allowable token types.
*/
#include <vector>
#include <fstream>

#include "Token.h"
using namespace std;

// Constants
const char LEFT_PAREN_CHAR		= '(';
const char RIGHT_PAREN_CHAR		= ')';
const char COMMA_CHAR			= ',';
const char PERIOD_CHAR			= '.';
const char Q_MARK_CHAR			= '?';
const char MULTIPLY_CHAR		= '*';
const char ADD_CHAR				= '+';
const char COLON_CHAR			= ':';
const char DASH_CHAR			= '-';
const char SINGLE_QUOTE_CHAR	= '\'';
const char HASH_CHAR			= '#';
const char LINE_CHAR			= '|';
const char NEW_LINE_CHAR		= '\n';

const string LEFT_PAREN_STR		= "(";
const string RIGHT_PAREN_STR	= ")";
const string COMMA_STR			= ",";
const string PERIOD_STR			= ".";
const string Q_MARK_STR			= "?";
const string MULTIPLY_STR		= "*";
const string ADD_STR			= "+";
const string COLON_STR			= ":";
const string SINGLE_QUOTE_STR	= "\'";
const string HAS_STR			= "#";
const string OPEN_COMMENT_STR	= "#|";
const string CLOSE_COMMENT_STR	= "|#";
const string COLON_DASH_STR		= ":-";
const string END_FILE_STR		= "";

class Scanner
{
public:
	Scanner(ifstream* inputFile, string inputFileName);
	~Scanner();
	void destroyTokens();
	string toStringTokens();
	string toStringTokenValues();
	void runScan();
	void printTokens();
	void writeOutputFile();
	vector<Token*> getTokens();


protected:
	map<string, tokenType> keywordMap = { 
		{ "Schemes", SCHEMES },
		{ "Facts", FACTS },
		{ "Rules", RULES },
		{ "Queries", QUERIES } 
	};
	int line = 1; // Always start at 1 instead of 0
	ifstream* inputFileStream;
	string inputFileName;
	vector<Token*> tokens;

private:
	// Helper Functions
	void analyzeChar(char ch);
	Token* tokenizeString();
	Token* tokenizeLineComment();
	Token* tokenizeMultiLineComment();
	Token* tokenizeKeywordOrID();
};

