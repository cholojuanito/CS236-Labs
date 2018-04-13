#pragma once

/*
* Tanner Davis March 1st 2018
* Interpreter.h
*
* A tuple represents the values in a row of the database.
*
*/
#include "DatalogProgram.h"
#include "Database.h"
#include "Graph.h"

using namespace std;

class Interpreter
{
public:
	Interpreter();
	Interpreter(DatalogProgram* program);
	~Interpreter();

	// Database creation
	void createDB();

	//Evaluation of Queries
	void interpret();

	// Debugging
	string toString();

protected:
	DatalogProgram* program;
	Database db;

private:
	Graph dependencyGraph;
	Graph reverseGraph;

	/*
	 * Helper data members
	 */

	// Query evaluation
	vector<int> columnsNumbersToProject;
	vector<string> queryValues;
	vector<string> variableNamesAdded;

	// Rule evaluation
	vector<string> allRuleHeadNames;
	string ruleHeadName;
	vector<string> ruleHeadValues;
	vector<string> ruleBodyNames;
	vector<vector<string>> ruleBodyValues;
	vector<set<int>> sccs; // Strongly Connected Components

	/*
	 * End helper data members 
	 */


	/* 
	 * Helper functions
	 */

	//Database creation
	void createAndAddTables();
	void createAndAddRows();
	Relation createTable(string name, vector<string> scheme);
	Scheme createColumnHeaders(vector<string> columnHeaders);
	Tuple createRow(vector<string> rowValues);

	// Query evaluation
	void evaluateQueries();
	void evaluateQuery(string queryScheme);
	void addToColumnNumsToProject(string variableName, int columnNumToAdd);
	void performConstantSelects(Relation& relationCopy);
	void performVariableSelects(Relation& relationCopy);
	void performProjectionAndRenaming(Relation& relationCopy);
	bool queryHasVariables();

	// Rule evaluation
	void createDependencyGraphs();
	void createNodesAndEdges();
	void addNodesToGraphs(int numRules);
	void createGraphEdges(int numRules, int currentRuleIndex);
	void findSCCs();

	void evaluateRulesBySCC();
	void evaluateRules(set<int>& scc);
	void evaluateRulesRelyOnSelf(set<int>& scc);
	void evaluateRulesOnce(set<int>& scc);
	void evaluateRule();
	bool numOfDBRowsHasChanged(int oldNumOfRows);
	bool shouldEvaluateSCCOnlyOnce(set<int>& scc);
	bool sccDependsOnItself(set<int>& scc);
	vector<Relation> performEvalOfRightSideRules();
	void performJoins(Relation& relationCopy, vector<Relation>& rightSideRelations);
	void getRuleColumnsToProject(vector<string>& endRelationSchemeVals);

	// Output
	string toStringQueryEvaluation(Relation& finalRelation, string querySchemeName);
	string toStringQuery(string querySchemeName);
	string toStringQueryWithVariables(set<Tuple>& rows);

	string toStringRule();
	string toStringHeadRule();
	string toStringRulePredicates();
	string toStringSCC(set<int> scc);

	/*
	 * End helper functions
	 */
};
