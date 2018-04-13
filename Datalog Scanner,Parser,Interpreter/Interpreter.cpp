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
	cout << "Dependency Graph" << endl;
	this->createDependencyGraphs();
	cout << "Rule Evaluation" << endl;
	this->evaluateRulesBySCC();
	cout << endl << "Query Evaluation" << endl;
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

void Interpreter::createDependencyGraphs() {
	this->allRuleHeadNames = this->program->getAllHeadRuleNames();
	this->createNodesAndEdges();

	cout << this->dependencyGraph.toString() << endl;
	// Find SCCs
	this->findSCCs();
}

void Interpreter::createNodesAndEdges() {
	int numRules = this->program->rules.size();
	// Add all nodes to graph
	this->addNodesToGraphs(numRules);
	// Add edges to nodes
	for (int i = 0; i < numRules; i++) {
		this->ruleHeadName = this->program->getHeadRuleName(i);
		this->ruleBodyNames = this->program->getRuleNames(i);

		this->createGraphEdges(numRules, i);
	}
}

void Interpreter::addNodesToGraphs(int numRules) {
	for (int i = 0; i < numRules; i++) {
		this->dependencyGraph.addNode(i);
		this->reverseGraph.addNode(i);
	}
}

void Interpreter::createGraphEdges(int numRules, int currentRuleIndex) {
	int numRuleNames = this->ruleBodyNames.size();

	for (int i = 0; i < numRuleNames; i++) {
		string name = this->ruleBodyNames.at(i);
		for (int j = 0; j < numRules; j++) {
			if (name == this->allRuleHeadNames.at(j)) {
				this->dependencyGraph.addEdge(currentRuleIndex, j);
				this->reverseGraph.addEdge(j, currentRuleIndex);
			}
		}

	}

}

void Interpreter::findSCCs() {
	// Step 1 - Run dfsForest on the reverse-edge graph
	this->reverseGraph.dfsForest();
	// Step 2 - Order the nodes by decreasing post-order number
	stack<int> orderedRules = this->reverseGraph.getPostOrderNums();
	// Step 3 - Run dfs(n) for each node in the dependency graph in order from last step
	this->dependencyGraph.setPostOrderNums(orderedRules);
	// Step 4 - Nodes reachable by dfs(n) form a component, gather all the components together
	this->sccs = this->dependencyGraph.findSCCs();
}

/*
 * Goes through each of the SCCs that were generated by
 * the dependency graph.
 * If statement determines which of the evaluateRules() overloads to use.
 */
void Interpreter::evaluateRulesBySCC() {
	for (set<int> nextSCC : this->sccs) {
		if (this->shouldEvaluateSCCOnlyOnce(nextSCC)) { // Evaluate with only one loop
			this->evaluateRulesOnce(nextSCC);
		}
		else { // Use a fixed point algorithm
			if (this->sccDependsOnItself(nextSCC)) { // Depends on itself
				this->evaluateRulesRelyOnSelf(nextSCC);
			}
			else { // Does not depend on itself
				this->evaluateRules(nextSCC);
			}
		}
	}

}

/*
 * Evaluate Rules - Overload #1
 * This version performs the evaluation process using a fixed-point algorithm.
 * Once the number of rows in the Database is uneffected by the evaluation of a rule
 * the loop ends.
 * Used when the SCC contains 2 or more rules to evaluate
 * i.e. SCC => { R0, R1, R2 }
*/
void Interpreter::evaluateRules(set<int>& scc) {

	string sccStr = this->toStringSCC(scc);
	int numItersToComplete = 0;
	bool newRowsAdded = false;

	cout << "SCC: ";
	cout << sccStr;
	do {
		int numOldRows = this->db.getTotalNumRows();

		for (int ruleNum : scc) {
			this->ruleHeadName = this->program->getHeadRuleName(ruleNum);
			this->ruleHeadValues = this->program->getHeadRuleValues(ruleNum);
			this->ruleBodyNames = this->program->getRuleNames(ruleNum);
			this->ruleBodyValues = this->program->getRuleValues(ruleNum);

			cout << this->toStringRule() << endl;

			this->evaluateRule();

		}

		newRowsAdded = this->numOfDBRowsHasChanged(numOldRows);
		numItersToComplete++;
	} while (newRowsAdded);

	cout << numItersToComplete << " passes: " << sccStr << endl;
}

/*
* Evaluate Rules - Overload #2
* This version is the same was Overload #1 because they are making me do another whole function just to get the correct output. YEP ITS STUPID.
* i.e. SCC => { R0 }
*/

void Interpreter::evaluateRulesRelyOnSelf(set<int>& scc) {
	string sccStr = this->toStringSCC(scc);
	int numItersToComplete = 0;
	bool newRowsAdded = false;

	cout << "SCC: ";
	cout << sccStr;
	do {
		int numOldRows = this->db.getTotalNumRows();

		for (int ruleNum : scc) {
			this->ruleHeadName = this->program->getHeadRuleName(ruleNum);
			this->ruleHeadValues = this->program->getHeadRuleValues(ruleNum);
			this->ruleBodyNames = this->program->getRuleNames(ruleNum);
			this->ruleBodyValues = this->program->getRuleValues(ruleNum);

			cout << this->toStringRule() << endl;

			this->evaluateRule();

		}

		newRowsAdded = this->numOfDBRowsHasChanged(numOldRows);
		numItersToComplete++;
	} while (newRowsAdded);

	cout << numItersToComplete << " passes: " << sccStr;
}

/*
 * Evaluate Rules - Overload #3
 * This version only performs one pass through the evaluation process.
 * Used when the SCC only has one rule to use and it does not rely on itself.
 * i.e. SCC => { R0 }
 */
void Interpreter::evaluateRulesOnce(set<int>& scc) {
	string sccStr = this->toStringSCC(scc);
	cout << "SCC: ";
	cout << sccStr;
	for (int ruleNum : scc) {
		this->ruleHeadName = this->program->getHeadRuleName(ruleNum);
		this->ruleHeadValues = this->program->getHeadRuleValues(ruleNum);
		this->ruleBodyNames = this->program->getRuleNames(ruleNum);
		this->ruleBodyValues = this->program->getRuleValues(ruleNum);

		cout << this->toStringRule() << endl;

		this->evaluateRule();

	}
	cout << "1 passes: " << sccStr;
}

void Interpreter::evaluateRule() {
	Relation endRelation;
	Relation relationFromDB = this->db.findTable(this->ruleHeadName);

	// Step 1: Evaluate predicates on right hand side of a rule declaration
	vector<Relation> rightSideRelations = this->performEvalOfRightSideRules();

	// Step 2: Join the resulting relations together to form one relation
	int numRightSideRelations = rightSideRelations.size();
	if (numRightSideRelations > 1) {
		this->performJoins(endRelation, rightSideRelations);
	}
	else {
		endRelation = rightSideRelations.at(0);
	}

	// Step 3: Project columns in head predicate of the rule
	vector<string> endRelationSchemeVals = endRelation.getScheme().getColumnHeaders();
	this->getRuleColumnsToProject(endRelationSchemeVals);
	endRelation = endRelation.project(this->columnsNumbersToProject);

	// Step 4: Rename relation to make it "unionable"
	endRelation = endRelation.rename(relationFromDB.getScheme());

	// Step 5: Union with relation in database
	relationFromDB.onion(endRelation);
	this->db.updateTable(this->ruleHeadName, relationFromDB);
}

bool Interpreter::numOfDBRowsHasChanged(int oldNumOfRows) {
	if (oldNumOfRows < this->db.getTotalNumRows()) {
		return true;
	}
	else {
		return false;
	}
}

bool Interpreter::shouldEvaluateSCCOnlyOnce(set<int>& scc) {
	if (scc.size() > 1) {
		return false;
	}
	else {
		if (this->sccDependsOnItself(scc)) { // Scc's rules depend on themselves
			return false;
		}
		else {
			return true;
		}
	}

}

bool Interpreter::sccDependsOnItself(set<int>& scc) {
	set<int>::iterator iter = scc.begin();
	set<int>::iterator last = scc.end();
	for (; iter != last; iter++) {
		set<int> adjacentNodes = this->dependencyGraph.getNodes().at(*iter).getIndexesOfAdjacentNodes();
		if (adjacentNodes.count(*iter) != 0) {
			return true;
		}
	}
	return false;
}

/*
 * Evaluate rules on right hand side of a rule declaration
 *
 * For each predicate on the right-hand side of a rule,
 * evaluate the predicate in the same way you evaluate the queries.
 * Each predicate should produce a single relation as an result.
 * If there are n predicates on the right-hand side of a rule, there should be n results.
 */
vector<Relation> Interpreter::performEvalOfRightSideRules() {
	vector<Relation> rightSideRelations = {};
	Relation newRelation;
	int numRules = this->ruleBodyNames.size();

	for (int i = 0; i < numRules; i++) {
		// Clear these before every rule
		this->columnsNumbersToProject.clear();
		this->variableNamesAdded.clear();

		vector<string> values = this->ruleBodyValues.at(i);
		newRelation = this->db.findTable(this->ruleBodyNames.at(i));
		int numVals = values.size();
		for (int j = 0; j < numVals; j++) {
			this->queryValues = values;

			this->columnsNumbersToProject.push_back(j);
		}

		this->variableNamesAdded = this->ruleBodyValues.at(i);
		this->performConstantSelects(newRelation);
		this->performVariableSelects(newRelation);
		this->performProjectionAndRenaming(newRelation);

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
		if (i == 1) { // Join first two together
			relationCopy = relationCopy.join(rightSideRelations.at(i - 1), rightSideRelations.at(i));
		}
		else { // Just join the end relation with the next relation in line
			relationCopy = relationCopy.join(relationCopy, rightSideRelations.at(i));
		}
	}
}

void Interpreter::getRuleColumnsToProject(vector<string>& endRelationSchemeVals) {
	int numEndSchemeVals = endRelationSchemeVals.size();
	int numColumnsToProject = this->ruleHeadValues.size();

	// Clear these so we can start fresh on projecting
	this->columnsNumbersToProject.clear();
	this->variableNamesAdded.clear();

	for (int i = 0; i < numColumnsToProject; i++) {
		string value = this->ruleHeadValues.at(i);

		for (int j = 0; j < numEndSchemeVals; j++) {
			if (value == endRelationSchemeVals.at(j)) {
				this->columnsNumbersToProject.push_back(j);
			}
		}
	}
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

string Interpreter::toStringRule() {
	ostringstream oss;

	oss << this->toStringHeadRule();
	oss << this->toStringRulePredicates();

	return oss.str();
}

string Interpreter::toStringHeadRule() {
	ostringstream oss;
	int numHeadRuleVals = this->ruleHeadValues.size();

	oss << ruleHeadName << "(";
	for (int i = 0; i < numHeadRuleVals; i++) {
		if (i < numHeadRuleVals - 1) {
			oss << this->ruleHeadValues.at(i) << ",";
		}
		else {
			oss << this->ruleHeadValues.at(i);
		}
	}
	oss << ") :- ";

	 return oss.str();
}
string Interpreter::toStringRulePredicates() {
	ostringstream oss;
	int numRuleNames = this->ruleBodyNames.size();

	for (int i = 0; i < numRuleNames; i++) {
		oss << this->ruleBodyNames.at(i) << "(";

		int numValues = this->ruleBodyValues.at(i).size();
		for (int k = 0; k < numValues; k++) {
			if (k < numValues - 1) {
				oss << this->ruleBodyValues.at(i).at(k) << ",";
			}
			else {
				oss << this->ruleBodyValues.at(i).at(k);
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

string Interpreter::toStringSCC(set<int> scc) {
	ostringstream oss;
	set<int>::iterator iter = scc.begin();
	set<int>::iterator last = scc.end();

	for (; iter != last; iter++) {
		if (next(iter) == last) {
			oss << "R" << *iter;
		}
		else {
			oss << "R" << *iter << ",";
		}
	}
	oss << endl;

	return oss.str();
}

string Interpreter::toString() {
	ostringstream oss;
	oss << this->db.toString();
	return oss.str();
}
