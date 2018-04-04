/*
 * Tanner Davis
 * 
 * 
 * 
 */
#pragma once
#include "Token.h"

class Parameter
{
public:
	Parameter(Token* token);
	~Parameter();
	void expression(Parameter* firstParam, Token* operatorToken, Parameter* secondParam);
	void terminal(Token* token);
	string toString();

	string getParamValue();

	// Terminal Token and Type
	tokenType type;
	Token* terminalToken;

	// Expression Parameters
	Parameter* firstParameter;
	Parameter* secondParameter;
	Token* operatorToken;

};

