#include "DatalogProgram.h"

DatalogProgram::DatalogProgram() {
	this->errorToken = nullptr;
}

DatalogProgram::~DatalogProgram() {
	for (unsigned int i = 0; i < this->schemes.size(); i++) {
		delete this->schemes.at(i);
	}
	for (unsigned int i = 0; i < this->facts.size(); i++) {
		delete this->facts.at(i);
	}
	for (unsigned int i = 0; i < this->rules.size(); i++) {
		delete this->rules.at(i);
	}
	for (unsigned int i = 0; i < this->queries.size(); i++) {
		delete this->queries.at(i);
	}
	delete this->errorToken;
}

vector<string> DatalogProgram::getSchemeNames() {
	vector<string> names;
	int numSchemes = this->schemes.size();
	for (int i = 0; i < numSchemes; i++) {
		//cout << "SCHEME AT I: " << endl;
		//cout << this->schemes.at(i)->id->getValue() << endl;
		names.push_back(this->schemes.at(i)->id->getValue());
	}

	return names;
}

vector<string> DatalogProgram::getFactNames() {
	vector<string> names;
	int numFacts = this->facts.size();
	for (int i = 0; i < numFacts; i++) {
		names.push_back(this->facts.at(i)->id->getValue());
	}
	return names;
}

string DatalogProgram::getHeadRuleName(int index) {
	return this->rules.at(index)->getHeadPredName();
}

vector<string> DatalogProgram::getRuleNames(int index) {
	vector<string> names;
	names = this->rules.at(index)->getPredNames();
	return names;
}

vector<string> DatalogProgram::getQueryNames() {
	vector<string> names;
	int numQueries = this->queries.size();
	for (int i = 0; i < numQueries; i++) {
		names.push_back(this->queries.at(i)->id->getValue());
	}
	return names;
}

vector<string> DatalogProgram::getSchemeValues(int index) {
	return this->schemes.at(index)->getParameterValues();
}

vector<string> DatalogProgram::getFactValues(int index) {
	return this->facts.at(index)->getParameterValues();
}

vector<string> DatalogProgram::getHeadRuleValues(int index) {
	return this->rules.at(index)->getHeadPredValues();
}

vector<vector<string>> DatalogProgram::getRuleValues(int index) {
	return this->rules.at(index)->getPredValues();
}

vector<string> DatalogProgram::getQueryValues(int index) {
	return this->queries.at(index)->getParameterValues();
}

void DatalogProgram::addScheme(Predicate* scheme) {
	this->schemes.push_back(scheme);
}

void DatalogProgram::addFact(Predicate* fact) {
	this->facts.push_back(fact);
}

void DatalogProgram::addRule(Rule* rule) {
	this->rules.push_back(rule);
}

void DatalogProgram::addQuery(Predicate* query) {
	this->queries.push_back(query);
}

void DatalogProgram::addDomainValue(string domainValue) {
	bool valueExists = false;
	int numDomainVals = this->domain.size();
	for (int i = 0; i < numDomainVals; i++) {
		if (domainValue == this->domain.at(i)) {
			valueExists = true;
			break;
		}
	}

	if (!valueExists) {
		this->domain.push_back(domainValue);
	}
}

void DatalogProgram::setError(Token* errorToken) {
	this->errorToken = errorToken;
}

string DatalogProgram::toString() {

	if (this->errorToken != nullptr) {
		return "Failure!\n" + this->errorToken->toString();
	}
	ostringstream oss;
	// Put the domain in descending order.
	this->sortDomain();

	oss << "Success!" << endl;
	oss << this->schemesToStr();
	oss << this->factsToStr();
	oss << this->rulesToStr();
	oss << this->queriesToStr();
	oss << this->domainToStr();

	return oss.str();
}

string DatalogProgram::schemesToStr() {
	ostringstream oss;
	int numSchemes = this->schemes.size();

	oss << "Schemes(" << numSchemes << "):" << endl;
	for (int i = 0; i < numSchemes; i++) {
		oss << "  " + this->schemes.at(i)->toString() << endl;
	}
	return oss.str();
}

string DatalogProgram::factsToStr() {
	ostringstream oss;
	int numFacts = this->facts.size();

	oss << "Facts(" << numFacts << "):" << endl;
	for (int i = 0; i < numFacts; i++) {
		oss << "  " + this->facts.at(i)->toString() << "." << endl;
	}
	return oss.str();
}

string DatalogProgram::rulesToStr() {
	ostringstream oss;
	int numRules = this->rules.size();

	oss << "Rules(" << numRules << "):" << endl;
	for (int i = 0; i < numRules; i++) {
		oss << "  " + this->rules.at(i)->toString() << "." << endl;
	}
	return oss.str();
}

string DatalogProgram::queriesToStr() {
	ostringstream oss;
	int numQueries = this->queries.size();

	oss << "Queries(" << numQueries << "):" << endl;
	for (int i = 0; i < numQueries; i++) {
		oss << "  " + this->queries.at(i)->toString() << "?" << endl;
	}
	return oss.str();
}

string DatalogProgram::domainToStr() {
	ostringstream oss;
	int domainSize = this->domain.size();

	oss << "Domain(" << domainSize << "):" << endl;
	for (int i = 0; i < domainSize; i++) {
		oss << "  " + this->domain.at(i) << endl;
	}
	return oss.str();
}

void DatalogProgram::sortDomain() {
	sort(this->domain.begin(), this->domain.end());
}
