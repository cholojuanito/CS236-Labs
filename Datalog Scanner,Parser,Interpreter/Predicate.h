#pragma once
#include <vector>
#include "Parameter.h"

class Predicate
{
public:
	Predicate(Token* id);
	~Predicate();
	void addParameter(Parameter* parameter);
	string toString();

	vector<string> getParameterValues();
	string getPredicateValue();

	Token* id;
	vector<Parameter*> parameterList;
};

