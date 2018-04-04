#include "Predicate.h"

Predicate::Predicate(Token* id) {
	this->id = id;
}

Predicate::~Predicate() {
	int numPredicates = this->parameterList.size();

	delete this->id;
	for (int i = 0; i < numPredicates; i++) {
		delete this->parameterList.at(i);
	}
}

void Predicate::addParameter(Parameter* parameter) {
	this->parameterList.push_back(parameter);
}

string Predicate::toString() {
	ostringstream oss;
	int numParams = this->parameterList.size();

	oss << this->id->getValue();
	oss << "(" + this->parameterList.at(0)->toString();
	for (int i = 1; i < numParams; i++) {
		oss << "," + this->parameterList.at(i)->toString();
	}
	oss << ")";
	return oss.str();
}

vector<string> Predicate::getParameterValues() {
	vector<string> paramValues;
	int numParams = this->parameterList.size();
	for (int i = 0; i < numParams; i++) {
		paramValues.push_back(this->parameterList.at(i)->terminalToken->getValue());
	}
	return paramValues;
}

string Predicate::getPredicateValue() {
	return this->id->getValue();
}
