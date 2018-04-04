/*
 *  Tanner Davis
 *  Scanner.cpp
 *  Jan 14th 2018
 *
 *  Implementation of the functions in Scanner.h
 */
#include "Scanner.h"

Scanner::Scanner(ifstream* inputFile, string inputFileName) {
	this->inputFileStream = inputFile;
	this->inputFileName = inputFileName;
}

Scanner::~Scanner() {
	// Memory deallocation gets handled by the parser
	//this->destroyTokens();
}

void Scanner::destroyTokens() {
	int numTokens = this->tokens.size();
	for (int i = 0; i < numTokens; i++) {
		delete this->tokens.at(i);
	}
	this->tokens.clear();
}

/*
 * Grabs the next char and calls the analyze function on it
 * if the char is not whitespace.
 */
void Scanner::runScan() {
	char ch;
	this->line = 1;

	while ( this->inputFileStream->get(ch) ) {
		//Skip whitespace and keep track of line numbers.
		if (isspace(ch)) {
			if (ch == NEW_LINE_CHAR) {
				this->line++;
			}
			continue;
		}
		this->analyzeChar(ch);
	}
	//Add the END_OF_FILE token.
	this->line++;
	this->tokens.push_back(new Token(END_OF_FILE, END_FILE_STR, this->line));
}

/*
 * Analyzes the char and decides which of the possible token types it is.
 * If it is a string, keyword, identifier or comment the respective
 * tokenize function is called.
 */
void Scanner::analyzeChar(char ch) {
	char nextChar;
	switch (ch) {

	case COMMA_CHAR:
		this->tokens.push_back(new Token(COMMA, COMMA_STR, this->line));
		break;

	case LEFT_PAREN_CHAR:
		this->tokens.push_back(new Token(LEFT_PAREN, LEFT_PAREN_STR, this->line));
		break;

	case RIGHT_PAREN_CHAR:
		this->tokens.push_back(new Token(RIGHT_PAREN, RIGHT_PAREN_STR, this->line));
		break;

	case PERIOD_CHAR:
		this->tokens.push_back(new Token(PERIOD, PERIOD_STR, this->line));
		break;

	case Q_MARK_CHAR:
		this->tokens.push_back(new Token(Q_MARK, Q_MARK_STR, this->line));
		break;

	case COLON_CHAR:
		if (this->inputFileStream->peek() == DASH_CHAR) {
			this->inputFileStream->get(nextChar);
			this->tokens.push_back(new Token(COLON_DASH, COLON_DASH_STR, this->line));
		}
		else {
			this->tokens.push_back(new Token(COLON, COLON_STR, this->line));
		}
		break;

	case MULTIPLY_CHAR:
		this->tokens.push_back(new Token(MULTIPLY, MULTIPLY_STR, this->line));
		break;

	case ADD_CHAR:
		this->tokens.push_back(new Token(ADD, ADD_STR, this->line));
		break;

	case SINGLE_QUOTE_CHAR:

		this->tokens.push_back(this->tokenizeString());
		break;

// Changes made here so that the parser doesn't have to deal with comments
	case HASH_CHAR:
		if (this->inputFileStream->peek() == LINE_CHAR) {
			this->inputFileStream->get(nextChar);
			//this->tokens.push_back(this->tokenizeMultiLineComment());
			delete this->tokenizeMultiLineComment();
		}
		else {
			//this->tokens.push_back(this->tokenizeLineComment());
			delete this->tokenizeLineComment();
		}
		break;

	default:
		if ( isalpha(ch) ) {
			// Put the char back then extract the keyword or id.
			this->inputFileStream->putback(ch);
			this->tokens.push_back(this->tokenizeKeywordOrID());
		}
		else {
			string unknowonToken = "";
			unknowonToken += ch;
			this->tokens.push_back(new Token(UNDEFINED, unknowonToken, this->line));
		}
		break;
	}

	return;
}

/*
 * Extracts the string's value and tokenizes it.
 */
Token* Scanner::tokenizeString() {
	string extractedStr = SINGLE_QUOTE_STR;
	int startLine = this->line;
	char ch;

	while (true) {
		//Get the next char
		this->inputFileStream->get(ch);
		// Unterminated string
		if (this->inputFileStream->eof()) {
			return new Token(UNDEFINED, extractedStr, startLine);
		}

		// End the string and tokenize it
		if (ch == SINGLE_QUOTE_CHAR) {
			if (this->inputFileStream->peek() == SINGLE_QUOTE_CHAR) {
				this->inputFileStream->get(ch);
				extractedStr += ch;
				continue;
			}
			extractedStr += SINGLE_QUOTE_CHAR;
			return new Token(STRING, extractedStr, startLine);
		}

		// Add a line if the string spans over multiple lines
		if (ch == NEW_LINE_CHAR) {
			this->line++;
		}
		// Concatenate the char to the string
		extractedStr += ch;
	}
}

/*
* Extracts the one-line-comment's value and tokenizes it.
*/
Token* Scanner::tokenizeLineComment() {
	string extractedComment = "#";
	int lineOfComment = this->line;
	char ch;
	while (true) {
		this->inputFileStream->get(ch);
		if (this->inputFileStream->eof()) {
			break;
		}

		if (ch == NEW_LINE_CHAR) {
			this->line++;
			break;
		}
		extractedComment += ch;
	}

	return new Token(COMMENT, extractedComment, lineOfComment);

}

/*
* Extracts the multi-line-comment's value and tokenizes it.
*/
Token* Scanner::tokenizeMultiLineComment() {
	string extractedComment = OPEN_COMMENT_STR;
	int startLine = this->line;
	char ch;
	while (true) {
		this->inputFileStream->get(ch);

		if (ch == LINE_CHAR && ( this->inputFileStream->peek() == HASH_CHAR)) {
			this->inputFileStream->get(ch);
			extractedComment += CLOSE_COMMENT_STR;
			return new Token(COMMENT, extractedComment, startLine);
		}

		if (this->inputFileStream->eof()) {
			return new Token(UNDEFINED, extractedComment, startLine);
		}

		// Add a line if the comment spans over multiple lines
		if (ch == NEW_LINE_CHAR) {
			this->line++;
		}
		extractedComment += ch;
	}
}

/*
* Extracts the keyword's or identifer's value and tokenizes it.
*/
Token* Scanner::tokenizeKeywordOrID() {
	string extractedStr;
	int startLine = this->line;
	char ch;

	while ( isalnum(this->inputFileStream->peek()) || isalpha(this->inputFileStream->peek()) ) {
		this->inputFileStream->get(ch);
		extractedStr += ch;
	}

	if ( this->keywordMap.find(extractedStr) != this->keywordMap.end() ) {
		return new Token(keywordMap[extractedStr], extractedStr, startLine);
	}
	else {
		return new Token(ID, extractedStr, startLine);
	}
}

/*
 * Outputs all the tokens the way the lab wants them.
 * (type,"value",line)
 */
void Scanner::printTokens() {
	int numTokens = this->tokens.size();
	for (int i = 0; i < numTokens; i++) {
		cout << this->tokens.at(i)->toString();
		if (i < numTokens - 1) {
			cout << endl;
		}
	}

	cout << endl;
	cout << "Total Tokens = " << numTokens;
}

/*
* Returns a string of all the tokens the way the lab wants them.
* (type,"value",line)
*/
string Scanner::toStringTokens() {
	ostringstream outStrStream;
	int numTokens = this->tokens.size();
	for (int i = 0; i < numTokens; i++) {
		outStrStream << this->tokens.at(i)->toString();
		if (i < numTokens - 1) {
			outStrStream << endl;
		}
	}
	outStrStream << endl;
	outStrStream << "Total Tokens = " << numTokens;

	return outStrStream.str();

}

/*
 * Returns a string of all the token values
 * seperated by a new line.
 */
string Scanner::toStringTokenValues() {
	ostringstream outStrStream;
	int numTokens = this->tokens.size();
	for (int i = 0; i < numTokens; i++) {
		outStrStream << this->tokens.at(i)->tokenValue;
		if (i < numTokens - 1) {
			outStrStream << endl;
		}
	}
	return outStrStream.str();
}


/*
 * Writes the tokens to a file with the prefix 'out_' attached to
 * the given input file's name.
 */
void Scanner::writeOutputFile() {
	string outputString = this->toStringTokens();
	string outputFileName =  "out_" + this->inputFileName;
	ofstream outputStream;
	outputStream.open(outputFileName);

	cout << endl;
	if (outputStream.is_open()) {
		cout << "Writing to file '" << outputFileName << "'..." << endl;
		outputStream << outputString;
		outputStream.close();
		cout << "Finished writing to file '" << outputFileName << "'." << endl;
	}
	else {
		cout << "There was an error while trying to create and write to file '" << outputFileName << "'" << endl;
	}

}

vector<Token*> Scanner::getTokens() {
	return this->tokens;
}
