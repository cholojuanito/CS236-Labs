#include "Interpreter.h"

Interpreter::Interpreter() {
}

Interpreter::Interpreter(DatalogProgram* program) {
	this->program = program;
}

Interpreter::~Interpreter() {
}

void Interpreter::createDB() {
	this->createAndAddTables();
	this->createAndAddRows();
}

void Interpreter::createAndAddTables() {
	string schemeName = "";
	int numSchemes = this->program->schemes.size();
	vector<string> schemeNames = this->program->getSchemeNames();
	vector<string> schemeValues;

	for (int i = 0; i < numSchemes; i++) {
		schemeName = schemeNames.at(i);
		schemeValues = this->program->getSchemeValues(i);
		Relation newRelation = this->createTable(schemeName, schemeValues);
		this->db.addTable(schemeName, newRelation);
	}
}

void Interpreter::createAndAddRows() {
	int numFacts = this->program->facts.size();
	string factScheme = "";
	vector<string> factNames = this->program->getFactNames();
	vector<string> factValues;

	for (int i = 0; i < numFacts; i++) {
		factScheme = factNames.at(i);
		factValues = this->program->getFactValues(i);
		Tuple newTuple = this->createRow(factValues);
		this->db.addRowToTable(factScheme, newTuple);
	}
}

Relation Interpreter::createTable(string name, vector<string> scheme) {
	Scheme newScheme = this->createColumnHeaders(scheme);
	return Relation(name, newScheme);
}

Scheme Interpreter::createColumnHeaders(vector<string> columnHeaders) {
	return Scheme(columnHeaders);
}

Tuple Interpreter::createRow(vector<string> attributeValues) {
	Tuple newTuple = Tuple();
	int numValues = attributeValues.size();

	for (int i = 0; i < numValues; i++) {
		newTuple.push_back(attributeValues.at(i));
	}

	return newTuple;
}

void Interpreter::interpret() {
	cout << "Rule Evaluation" << endl;
	this->evaluteRules();
	cout << "Query Evaluation" << endl;
	this->evaluateQueries();
}

void Interpreter::evaluateQueries() {
	int numQueries = this->program->queries.size();
	string queryScheme = "";
	vector<string> queryNames = this->program->getQueryNames();

	for (int i = 0; i < numQueries; i++) {
		queryScheme = queryNames.at(i);
		this->queryValues = this->program->getQueryValues(i);
		this->evaluateQuery(queryScheme);
	}
}

void Interpreter::evaluateQuery(string querySchemeName) {
	Relation copyRelation = this->db.findTable(querySchemeName);
	// Reset these vectors for every query
	this->variableNamesAdded.clear();
	this->columnsNumbersToProject.clear();

	this->performConstantSelects(copyRelation);
	this->performVariableSelects(copyRelation);
	this->performProjectionAndRenaming(copyRelation);

	cout << this->toStringQueryEvaluation(copyRelation, querySchemeName);


}

void Interpreter::addToColumnNumsToProject(int columnNumToAdd) {

}

void Interpreter::addToColumnNumsToProject(string variableName, int columnNumToAdd) {
	bool isDuplicate = false;
	int numAddedVariables = this->variableNamesAdded.size();

	for (int i = 0; i < numAddedVariables; i++) {
		if (variableName == this->variableNamesAdded.at(i)) {
			isDuplicate = true;
		}
	}

	if (!isDuplicate) {
		this->columnsNumbersToProject.push_back(columnNumToAdd);
		this->variableNamesAdded.push_back(variableName);
	}
}


void Interpreter::performConstantSelects(Relation& relationCopy) {
	int numQueryVals = this->queryValues.size();
	char singleQuote = '\'';

	for (int i = 0; i < numQueryVals; i++) {
		string current = this->queryValues.at(i);
		// If the query value is a constant just grab the rows that match that constant and it's column number
		if (current.at(0) == singleQuote) {
			relationCopy = relationCopy.select(current, i);
		}
		// If it is a variable add it to the list of column indexes that need to be projected
		else {
			this->addToColumnNumsToProject(current, i);
		}
	}
}

void Interpreter::performVariableSelects(Relation& relationCopy) {
	int numQueryVals = this->queryValues.size();

	for (int i = 0; i < numQueryVals; i++) {
		string queryVar1 = this->queryValues.at(i);
		for (int j = i + 1; j < numQueryVals; j++) {
			string queryVar2 = this->queryValues.at(j);
			if (queryVar1 == queryVar2) {
				relationCopy = relationCopy.select(i, j);
			}
		}
	}
}

void Interpreter::performProjectionAndRenaming(Relation& relationCopy) {
	// If there are any variables then project and rename the columns accordingly
	if (this->columnsNumbersToProject.size() > 0) {
		Scheme newScheme = this->createColumnHeaders(this->variableNamesAdded);
		relationCopy = relationCopy.project(this->columnsNumbersToProject);
		relationCopy = relationCopy.rename(newScheme);
	}
}

bool Interpreter::queryHasVariables() {
	if (this->variableNamesAdded.size() > 0) {
		return true;
	}
	else {
		return false;
	}
}

void Interpreter::evaluteRules() {
	int numRules = this->program->rules.size();
	int numTimesToComplete = 0;
	bool newRowsAdded = false;

	do {
		int numOldRows = this->db.getTotalNumRows();

		for (int i = 0; i < numRules; i++) {
			string headRuleName = this->program->getHeadRuleName(i);
			vector<string> headRuleValues = this->program->getHeadRuleValues(i);
			vector<string> ruleNames = this->program->getRuleNames(i);
			vector<vector<string>> ruleValues = this->program->getRuleValues(i);

			cout << this->toStringRule(headRuleName, headRuleValues, ruleNames, ruleValues);

			this->evaluteRule(headRuleName, headRuleValues, ruleNames, ruleValues);

		}

		newRowsAdded = this->numOfDBRowsHasChanged(numOldRows);
		numTimesToComplete++;
	} while (newRowsAdded);

	cout << endl << "Schemes populated after " << numTimesToComplete << " passes through the Rules." << endl << endl;
}

void Interpreter::evaluteRule(string ruleHeadName, vector<string> ruleHeadValues, vector<string> ruleNames, vector<vector<string>> ruleValues) {
	Relation endRelation, newRelation;
	Relation relationFromDB = this->db.findTable(ruleHeadName);

	// Step 1: Evaluate predicates on right hand side of a rule declaration
	vector<Relation> rightSideRelations = this->performEvalOfRightSideRules(ruleNames, ruleValues);

	// Step 2: Join the resulting relations together to form one relation
	int numRightSideRelations = rightSideRelations.size();
	//cout << "NUM RIGHT SIDE RELATIONS: " << numRightSideRelations << endl;
	if (numRightSideRelations > 1) {
		//cout << "JOINING" << endl;
		this->performJoins(endRelation, rightSideRelations);
	}
	else {
		endRelation = rightSideRelations.at(0);
	}

	//cout << "END RELATION AFTER JOIN: " << endl << endRelation.toString() << endl;
	// Step 3: Project columns in head predicate of the rule

	//cout << "PROJECTING" << endl;

	vector<string> endRelationSchemeVals = endRelation.getScheme().getColumnHeaders();
	this->getRuleColumnsToProject(ruleHeadValues, endRelationSchemeVals);


	endRelation = endRelation.project(this->columnsNumbersToProject);

	//cout << endl << "END RELATION AFTER PROJECTING: " << endl << endRelation.toString() << endl;
	// Step 4: Rename relation to make it "unionable"

	//cout << "RENAMING" << endl;
	endRelation = endRelation.rename(relationFromDB.getScheme());

	//cout << endl << "END RELATION AFTER RENAMING: " << endRelation.toString() << endl;

	//cout << "PERSISTING TO DATABASE" << endl;
	// Step 5: Union with relation in database
	relationFromDB.onion(endRelation);
	this->db.updateTable(ruleHeadName, relationFromDB);
	//cout << "AFTER UNION: " << endl << relationFromDB.toString() << endl;
}

bool Interpreter::numOfDBRowsHasChanged(int oldNumOfRows) {
	if (oldNumOfRows < this->db.getTotalNumRows()) {
		return true;
	}
	else {
		return false;
	}
}

/*
 * Evaluate rules on right hand side of a rule declaration
 *
 * For each predicate on the right-hand side of a rule,
 * evaluate the predicate in the same way you evaluate the queries.
 * Each predicate should produce a single relation as an result.
 * If there are n predicates on the right-hand side of a rule, there should be n results.
 */
vector<Relation> Interpreter::performEvalOfRightSideRules(vector<string>& ruleNames, vector<vector<string>>& ruleValues) {
	vector<Relation> rightSideRelations = {};
	Relation newRelation;
	int numRules = ruleNames.size();

	for (int i = 0; i < numRules; i++) {
		// Clear these before every rule
		this->columnsNumbersToProject.clear();
		this->variableNamesAdded.clear();

		vector<string> values = ruleValues.at(i);
		newRelation = this->db.findTable(ruleNames.at(i));

		//cout << "STEP 1 \n VALUES THAT SHOULD BE PROJECTED" << endl;
		int numVals = values.size();
		for (int j = 0; j < numVals; j++) {
			this->queryValues = values;
			//cout << ruleValues.at(i).at(j) << endl;

			this->columnsNumbersToProject.push_back(j);
		}

		this->variableNamesAdded = ruleValues.at(i);
		this->performConstantSelects(newRelation);
		this->performVariableSelects(newRelation);
		this->performProjectionAndRenaming(newRelation);



		//cout << "THE NEW RELATION FOR: " << ruleNames.at(i) << endl;
		//cout << newRelation.toString() << endl;



		rightSideRelations.push_back(newRelation);

	}

	return rightSideRelations;
}

/*
 * Join resulting relations
 *
 * If there are two or more predicates on the right-hand side of a rule,
 * join the intermediate results to form the single result for Step 2.
 * Thus, if p1, p2, and p3 are the intermediate results from Step 1, join them (p1 |x| p2 |x| p3) into a single relation.
 *
 * If there is a single predicate on the right hand side of the rule, use the single intermediate result from Step 1 as the result for Step 2.
 *
 */
void Interpreter::performJoins(Relation& relationCopy, vector<Relation>& rightSideRelations) {
	int numNewRelations = rightSideRelations.size();
	for (int i = 1; i < numNewRelations; i++) {
		if (i == 1) {
			// Join first two together
			//cout << "JOINING RELATION " << rightSideRelations.at(i - 1).getName() << " TO " << rightSideRelations.at(i).getName() << endl;

			//cout << "RELATION " << rightSideRelations.at(i - 1).getName() << ": " << endl << rightSideRelations.at(i - 1).toString() << endl << endl;
			//cout << "RELATION " << rightSideRelations.at(i).getName() << ": " << endl << rightSideRelations.at(i).toString() << endl;

			relationCopy = relationCopy.join(rightSideRelations.at(i - 1), rightSideRelations.at(i));
		}
		else { // Just join the end relation with the next relation in line
			//cout << "JOINING END RELATION TO " << rightSideRelations.at(i).getName() << endl;
			relationCopy = relationCopy.join(relationCopy, rightSideRelations.at(i));
		}
	}
}

void Interpreter::getRuleColumnsToProject(vector<string>& ruleHeadVals, vector<string>& endRelationSchemeVals) {
	int numEndSchemeVals = endRelationSchemeVals.size();
	int numColumnsToProject = ruleHeadVals.size();

	// Clear these so we can start fresh on projecting
	this->columnsNumbersToProject.clear();
	this->variableNamesAdded.clear();

	//cout << "NUM RULE HEAD VALS: " << numColumnsToProject << endl;
	//cout << "NUM ENDSCHEME: " << numEndSchemeVals << endl;

	//cout << "SHOULD PROJECT COLUMNS ";
	for (int i = 0; i < numColumnsToProject; i++) {
		string value = ruleHeadVals.at(i);

		// NOT NECESSARY
		//transform(value.begin(), value.end(), value.begin(), ::toupper); // Change the copy of the value to upper case to match the scheme's case
		for (int j = 0; j < numEndSchemeVals; j++) {
			//cout << "COMPARING COLUMN " << value << " WITH COLUMN " << endRelationSchemeVals.at(j) << endl;
			if (value == endRelationSchemeVals.at(j)) {
				//cout << j << " ";
				this->columnsNumbersToProject.push_back(j);
			}
		}
	}
	//cout << endl;
}

string Interpreter::toStringQueryEvaluation(Relation& finalRelation, string querySchemeName) {
	ostringstream oss;
	int numOfRows = finalRelation.getNumOfRows();
	set<Tuple> rows = finalRelation.getRows();
	bool hasVariables = this->queryHasVariables();

	oss << this->toStringQuery(querySchemeName);
	if (numOfRows <= 0) {
		oss << "No" << endl;
	}
	else {
		oss << "Yes(" << numOfRows << ")" << endl;
		if (hasVariables) {
			// Two spaces of indentation
			oss << this->toStringQueryWithVariables(rows);
		}
	}

	return oss.str();
}

string Interpreter::toStringQuery(string querySchemeName) {
	ostringstream oss;
	int numQueryParams = this->queryValues.size();
	int lastParam = numQueryParams - 1;

	oss << querySchemeName << "(";
	for (int i = 0; i < lastParam; i++) {
		oss << this->queryValues.at(i) << ",";
	}
	oss << this->queryValues.at(lastParam) << ")? ";

	return oss.str();
}

string Interpreter::toStringQueryWithVariables(set<Tuple>& rows) {
	ostringstream oss;
	set<Tuple>::iterator iter = rows.begin();
	set<Tuple>::iterator last = rows.end();
	int numVariables = this->variableNamesAdded.size();

	for (; iter != last; iter++) {
		oss << "  ";
		for (int i = 0; i < numVariables; i++) {
			oss << this->variableNamesAdded.at(i) << "=";
			if (i == numVariables - 1) {
				oss << iter->at(i);
			}
			else {
				oss << iter->at(i) << ", ";
			}
		}
		if (iter != last) {
			oss << endl;
		}
	}

	return oss.str();
}

string Interpreter::toStringRule(string& ruleHeadName, vector<string>& ruleHeadValues, vector<string>& ruleNames, vector<vector<string>>& ruleValues) const {
	ostringstream oss;

	oss << this->toStringHeadRule(ruleHeadName, ruleHeadValues);
	oss << this->toStringRulePredicates(ruleNames, ruleValues);
	oss << endl;

	return oss.str();
}

string Interpreter::toStringHeadRule(string& ruleHeadName, vector<string>& ruleHeadValues) const {
	ostringstream oss;
	int numHeadRuleVals = ruleHeadValues.size();

	oss << ruleHeadName << "(";
	for (int i = 0; i < numHeadRuleVals; i++) {
		if (i < numHeadRuleVals - 1) {
			oss << ruleHeadValues.at(i) << ",";
		}
		else {
			oss << ruleHeadValues.at(i);
		}
	}
	oss << ") :- ";

	 return oss.str();
}
string Interpreter::toStringRulePredicates(vector<string>& ruleNames, vector<vector<string>>& ruleValues) const {
	ostringstream oss;
	int numRuleNames = ruleNames.size();
	
	for (int i = 0; i < numRuleNames; i++) {
		oss << ruleNames.at(i) << "(";

		int numValues = ruleValues.at(i).size();
		for (int k = 0; k < numValues; k++) {
			if (k < numValues - 1) {
				oss << ruleValues.at(i).at(k) << ",";
			}
			else {
				oss << ruleValues.at(i).at(k);
			}
		}

		if (i < numRuleNames - 1) {
			oss << ")" << ",";
		}
		else {
			oss << ")";
		}
	}

	return oss.str();
}

string Interpreter::toString() {
	ostringstream oss;
	oss << this->db.toString();
	return oss.str();
}
