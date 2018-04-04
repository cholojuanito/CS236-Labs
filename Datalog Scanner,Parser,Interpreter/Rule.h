#pragma once

#include "Predicate.h"
class Rule
{
public:
	Rule();
	~Rule();
	void setHeadPredicate(Predicate* headPredicate);
	void addPredicate(Predicate* predicate);
	string toString();
	string getHeadPredName();
	vector<string> getHeadPredValues();
	vector<string> getPredNames();
	vector<vector<string>> getPredValues();

	Predicate* headPredicate;
	vector<Predicate*> predicateList;
};

