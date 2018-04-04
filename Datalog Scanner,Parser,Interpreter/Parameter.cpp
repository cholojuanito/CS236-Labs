#include "Parameter.h"

Parameter::Parameter(Token* token) {
	this->terminalToken = nullptr;
	this->firstParameter = nullptr;
	this->secondParameter = nullptr;
	this->operatorToken = nullptr;
	if (token != nullptr) {
		this->terminal(token);
	}
}

Parameter::~Parameter() {
	if (this->terminalToken != nullptr) {
		delete this->terminalToken;
	}

	if (this->firstParameter != nullptr) {
		delete this->firstParameter;
	}

	if (this->secondParameter != nullptr) {
		delete this->secondParameter;
	}

	if (this->operatorToken != nullptr) {
		delete this->operatorToken;
	}
}

void Parameter::expression(Parameter* firstParam, Token* operatorToken, Parameter* secondParam) {
	this->type = EXPRESSION;
	this->firstParameter = firstParam;
	this->secondParameter = secondParam;
	this->operatorToken = operatorToken;
}

void Parameter::terminal(Token* token) {
	this->type = token->getType();
	this->terminalToken = token;
}

string Parameter::toString() {
	if (this->type == EXPRESSION) {
		return "(" + this->firstParameter->toString() + this->operatorToken->getValue() + this->secondParameter->toString() + ")";
	}
	return this->terminalToken->getValue();
}

string Parameter::getParamValue() {
	if (this->type == EXPRESSION) {
		return  this->firstParameter->getParamValue() + this->operatorToken->getValue() + this->secondParameter->getParamValue();
	}
	return this->terminalToken->getValue();
}
