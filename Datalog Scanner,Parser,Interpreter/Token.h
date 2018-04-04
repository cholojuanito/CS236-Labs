#pragma once
/*
*  Tanner Davis
*  Token.h
*  Jan 14th 2018
*
*  Token class:
*  A token for Datalog language involves:
*		1) A token type
*		2) The token's value
*		3) The line number it is found on in the file
*
*/
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include <sstream>
using namespace std;

/*
 * Defines the types of tokens that can be accepted.
 */
enum tokenType {
	COMMA,
	PERIOD,
	Q_MARK,
	LEFT_PAREN,
	RIGHT_PAREN,
	COLON,
	COLON_DASH,
	MULTIPLY,
	ADD,
	SCHEMES,
	FACTS,
	RULES,
	QUERIES,
	ID,
	STRING,
	COMMENT,
	WHITESPACE,
	UNDEFINED,
	END_OF_FILE,
	EXPRESSION,
	ERROR
};

class Token
{
	friend class Scanner;
	public:
		Token(tokenType type, string value, int line);
		~Token();
		string toString();
		tokenType getType();
		string getValue();
		int getLine();

		map<tokenType, string> typeToStringMap { 
			{ COMMA, "COMMA" },
			{ PERIOD, "PERIOD" },
			{ Q_MARK, "Q_MARK" },
			{ LEFT_PAREN, "LEFT_PAREN" },
			{ RIGHT_PAREN, "RIGHT_PAREN" },
			{ COLON, "COLON" },
			{ COLON_DASH, "COLON_DASH" },
			{ MULTIPLY, "MULTIPLY" },
			{ ADD, "ADD" },
			{ SCHEMES, "SCHEMES" },
			{ FACTS, "FACTS" },
			{ RULES, "RULES" },
			{ QUERIES, "QUERIES" },
			{ ID, "ID" },
			{ STRING, "STRING" },
			{ COMMENT, "COMMENT" },
			{ WHITESPACE, "WHITESPACE" },
			{ UNDEFINED, "UNDEFINED" },
			{ END_OF_FILE, "EOF" } 
		};

	protected:
		tokenType type;
		string tokenValue;
		int lineNumber;
};

