#include "Rule.h"

Rule::Rule() {
	this->headPredicate = nullptr;
}

/*
 * Deallocate memory
 */
Rule::~Rule() {
	int numPredicates = this->predicateList.size();

	delete this->headPredicate;
	for (int i = 0; i < numPredicates; i++) {
		delete this->predicateList.at(i);
	}
}

/*
 * Set head predicate value
 */
void Rule::setHeadPredicate(Predicate* headPredicate) {
	this->headPredicate = headPredicate;
}

/*
 * Add a predicate pointer to the list of predicates
 */
void Rule::addPredicate(Predicate* predicate) {
	this->predicateList.push_back(predicate);
}

/*
 * Convert the Rule into a string
 */
string Rule::toString() {
	ostringstream oss;
	int numPredicates = this->predicateList.size();

	oss << this->headPredicate->toString() + " :- " + this->predicateList.at(0)->toString();
	for (int i = 1; i < numPredicates; i++) {
		oss << "," + this->predicateList.at(i)->toString();
	}
	return oss.str();
}

 string Rule::getHeadPredName() {
	 //cout << "HEAD PRED NAME: " << this->headPredicate->getPredicateValue() << endl;
	 return this->headPredicate->getPredicateValue();
 }

 vector<string> Rule::getHeadPredValues() {
	 vector<string> values = this->headPredicate->getParameterValues();

	 /*cout << "HEAD PRED VALUES: " << endl;
	 for (int i = 0; i < values.size(); i++) {
		 cout << values.at(i) << endl;
	 }*/

	 return values;
 }

 vector<string> Rule::getPredNames() {
	 vector<string> names = {};
	 //cout << "PRED NAMES: " << endl;
	 for (auto predicate : this->predicateList) {
		 //cout << predicate->getPredicateValue() << endl;
		 names.push_back(predicate->getPredicateValue());
	 }
	 return names;
 }

 vector<vector<string>> Rule::getPredValues() {
	 vector<vector<string>> values;

	 for (auto predicate : this->predicateList) {
		 values.push_back(predicate->getParameterValues());
	 }
	
	/* cout << "PRED VALUES: " << endl;
	 for (int i = 0; i < values.size(); i++) {
		 cout << values.at(i) << endl;
	 }*/

	 return values;
 }
