#pragma once
/*
 * Tanner Davis
 *  Parser.h
 *  Jan 14th 2018
 *
 *  Parser class:
 *  Parses through tokens and determines their validity.
 *  Throws an exception for any errors found in the tokens.
 */
#include <vector>
#include "Token.h"
#include "DatalogProgram.h"
#include "Rule.h"
#include "Predicate.h"
#include "Parameter.h"

class Parser
{
public:
	Parser(vector<Token*> tokens);
	~Parser();
	DatalogProgram* createProgram();

protected:
	vector<Token*> tokens;
	DatalogProgram* program;
	vector<Parameter*> expressionsToDelete; // This is used in case of an exception while recursively calling expression()

private:
	// Helper functions
	Token* currentToken();
	tokenType currentTokenType();
	Token* consumeToken(tokenType type);
	// Grammar functions
	void parse();
	void scheme();
	void fact();
	void rule();
	void query();

	void schemeList();
	void factList();
	void ruleList();
	void queryList();

	Predicate* headPredicate();
	Predicate* predicate();

	void predicateList(Rule* rule);
	void parameterList(Predicate* predicate);
	void stringList(Predicate* predicate);
	void idList(Predicate* predicate);

	Parameter* parameter();
	Parameter* expression();
	Token* parseOperator();

	void deleteRemainingTokens();
};
