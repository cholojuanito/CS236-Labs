#pragma once
/*
* Tanner Davis
*/
#include "Token.h"
#include "Predicate.h"
#include "Rule.h"
#include <algorithm>
#include <vector>

class DatalogProgram
{
public:
	DatalogProgram();
	~DatalogProgram();
	vector<string> getSchemeNames();
	vector<string> getFactNames();
	string getHeadRuleName(int index);
	vector<string> getAllHeadRuleNames();
	vector<string> getRuleNames(int index);
	vector<string> getQueryNames();

	vector<string> getSchemeValues(int index);
	vector<string> getFactValues(int index);
	vector<string> getHeadRuleValues(int index);
	vector<vector<string>> getRuleValues(int index);
	vector<string> getQueryValues(int index);

	void addScheme(Predicate* scheme);
	void addFact(Predicate* fact);
	void addRule(Rule* rule);
	void addQuery(Predicate* query);
	void addDomainValue(string domainValue);
	void setError(Token* errorToken);
	string toString();

	vector<Predicate*> schemes;
	vector<Predicate*> facts;
	vector<Rule*> rules;
	vector<Predicate*> queries;
	vector<string> domain;
	Token* errorToken;

private:
	// Helper Functions
	void sortDomain();
	string schemesToStr();
	string factsToStr();
	string rulesToStr();
	string queriesToStr();
	string domainToStr();

};
