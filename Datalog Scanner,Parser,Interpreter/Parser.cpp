#include "Parser.h"



Parser::Parser(vector<Token*> tokens) {
	this->tokens = tokens;
	this->program = nullptr;
}


Parser::~Parser() {}

DatalogProgram* Parser::createProgram() {
	// Make sure there is only one DatalogProgram ptr at a time
	if (this->program != nullptr) {
		delete this->program;
	}
	this->program = new DatalogProgram();

	try {
		this->parse();
		for (unsigned int i = 0; i < this->tokens.size(); i++) {
			delete this->tokens.at(i);
		}
	}
	catch (Token* errorToken) {
		//cout << "FAILURE CAUSED BY: " << errorToken->toString() << endl;
		this->program->setError(errorToken);
		this->deleteRemainingTokens();
		for (unsigned int i = 0; i < this->expressionsToDelete.size(); i++) {
			delete this->expressionsToDelete.at(i);
		}
	}

	return this->program;
}

Token* Parser::currentToken() {
	return this->tokens.at(0);
}

tokenType Parser:: currentTokenType() {
	return this->tokens.at(0)->getType();
}

Token* Parser::consumeToken(tokenType type) {
	//cout << "CURRENT TYPE SHOULD BE: " << this->typeToStringMap[type] << endl;


	if (this->currentTokenType() != type) {
		throw this->currentToken();
	}
	// Consume the token
	//cout << "CONSUMING: " << this->currentToken()->toString() << endl;
	Token* temp = this->tokens.at(0);
	this->tokens.erase(this->tokens.begin());
	// Delete any token that isn't related to a value, expression or identifier
	if (type != ID && type != STRING && type != ADD && type != MULTIPLY) {
		delete temp;
	}

	return temp;
}

void Parser::parse() {
	this->consumeToken(SCHEMES);
	this->consumeToken(COLON);

	this->scheme();
	this->schemeList();

	this->consumeToken(FACTS);
	this->consumeToken(COLON);

	this->factList();

	this->consumeToken(RULES);
	this->consumeToken(COLON);

	this->ruleList();

	this->consumeToken(QUERIES);
	this->consumeToken(COLON);

	this->query();
	this->queryList();
}

void Parser::scheme() {
	Predicate* scheme = new Predicate(this->consumeToken(ID));
	this->program->addScheme(scheme);

	this->consumeToken(LEFT_PAREN);

	scheme->addParameter(new Parameter(this->consumeToken(ID)));
	this->idList(scheme);

	this->consumeToken(RIGHT_PAREN);
}

void Parser::fact() {
	Predicate* fact = new Predicate(this->consumeToken(ID));
	this->program->addFact(fact);

	this->consumeToken(LEFT_PAREN);

	Token* strToken = this->consumeToken(STRING);
	this->program->addDomainValue(strToken->getValue());
	fact->addParameter( new Parameter(strToken) );
	this->stringList(fact);

	this->consumeToken(RIGHT_PAREN);
	this->consumeToken(PERIOD);
}

void Parser::rule() {
	Rule* rule = new Rule();
	this->program->addRule(rule);

	rule->setHeadPredicate(this->headPredicate());
	this->consumeToken(COLON_DASH);

	rule->addPredicate(this->predicate());
	this->predicateList(rule);

	this->consumeToken(PERIOD);
}

void Parser::query() {
	Predicate* query = this->predicate();
	this->program->addQuery(query);

	this->consumeToken(Q_MARK);
}

void Parser::schemeList() {
	if (this->currentTokenType() == ID) {
		this->scheme();
		this->schemeList();
	}
}

void Parser::factList() {
	if (this->currentTokenType() == ID) {
		this->fact();
		this->factList();
	}
}

void Parser::ruleList() {
	if (this->currentTokenType() == ID) {
		this->rule();
		this->ruleList();
	}
}

void Parser::queryList() {
	if (this->currentTokenType() == ID) {
		this->query();
		this->queryList();
	}
}

Predicate* Parser::headPredicate() {
	Token* id = this->consumeToken(ID);
	Predicate* headPred = new Predicate(id);

	this->consumeToken(LEFT_PAREN);

	headPred->addParameter( new Parameter(this->consumeToken(ID)) );
	this->idList(headPred);

	this->consumeToken(RIGHT_PAREN);

	return headPred;
}

Predicate* Parser::predicate() {
	Token* id = this->consumeToken(ID);
	Predicate* pred = new Predicate(id);
	try {
		this->consumeToken(LEFT_PAREN);

		pred->addParameter(this->parameter());
		this->parameterList(pred);

		this->consumeToken(RIGHT_PAREN);
	}
	catch(Token* errorToken) {
	}

	return pred;
}


void Parser::predicateList(Rule* rule) {
	if (this->currentTokenType() == COMMA) {
		this->consumeToken(COMMA);
		rule->addPredicate(this->predicate());
		this->predicateList(rule);
	}
}

void Parser::parameterList(Predicate* predicate) {
	if (this->currentTokenType() == COMMA) {
		this->consumeToken(COMMA);
		predicate->addParameter(this->parameter());
		this->parameterList(predicate);
	}
}

void Parser::stringList(Predicate* predicate) {
	if (this->currentTokenType() == COMMA) {
		this->consumeToken(COMMA);
		// Add the string to the programs domain
		Token* strToken = this->consumeToken(STRING);
		this->program->addDomainValue(strToken->getValue());
		predicate->addParameter( new Parameter(strToken) );
		this->stringList(predicate);
	}
}

void Parser::idList(Predicate* predicate) {
	if (this->currentTokenType() == COMMA) {
		this->consumeToken(COMMA);
		predicate->addParameter( new Parameter(this->consumeToken(ID)) );
		this->idList(predicate);
	}
}

Parameter* Parser::parameter() {
	try{
		if (this->currentTokenType() == STRING) {
			Token* str = this->consumeToken(STRING);
			return new Parameter(str);
		}
		else if (this->currentTokenType() == ID) {
			Token* id = this->consumeToken(ID);
			return new Parameter(id);
		}
		else {
			return this->expression();
		}
	}
	catch(Token* eToken) {}

	// Return an empty param in case an exception gets thrown
	return new Parameter(nullptr);
}

Parameter* Parser::expression() {

	Parameter* expression = new Parameter(nullptr);
	Parameter* firstParam = nullptr;
	Token* operatorToken = nullptr;
	Parameter* secondParam = nullptr;

	try {
		this->consumeToken(LEFT_PAREN);
		firstParam = this->parameter();
		operatorToken = this->parseOperator();
		secondParam = this->parameter();

		expression->expression(firstParam, operatorToken, secondParam);


		this->consumeToken(RIGHT_PAREN);
	}
	catch(Token* eToken) {
		if (firstParam != nullptr) {
			delete firstParam;
		}
		if (secondParam != nullptr) {
			delete secondParam;
		}
		//this->expressionsToDelete.push_back(expression);  //Be prepared to delete this one if an exception is thrown
	}


	return expression;
}

Token* Parser::parseOperator() {
	try {
		if (this->currentTokenType() == MULTIPLY) {
			return this->consumeToken(MULTIPLY);
		}
		else {
			return this->consumeToken(ADD);
		}
	}
	catch(Token* eToken) {
		throw eToken;
	}
	// This line should never be reached. It is just here for the complier
	return new Token(ERROR, "", 0);
}

void Parser::deleteRemainingTokens() {
	for (unsigned int i = 1; i < this->tokens.size(); i++) {
		delete this->tokens.at(i);
	}
}
