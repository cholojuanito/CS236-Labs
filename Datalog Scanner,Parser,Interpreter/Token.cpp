/*
*  Tanner Davis
*  Token.cpp
*  Jan 14th 2018
*
*  Implementation of the functions in Token.h
*/
#include "Token.h"

Token::Token(tokenType type, string value, int line) {
	this->type = type;
	this->tokenValue = value;
	this->lineNumber = line;
}

Token::~Token() {

}

/*
 * Create a string of the token following the form: (type, "value", line).
 */
string Token::toString() {
	stringstream ss;
	ss << this->lineNumber;
	return "(" + this->typeToStringMap[this->type] + ",\"" + this->tokenValue + "\"," + ss.str() + ")";
}

 tokenType Token::getType()
 {
	 return this->type;
 }

 string Token::getValue()
 {
	 return this->tokenValue;
 }

 int Token::getLine()
 {
	 return this->lineNumber;
 }
