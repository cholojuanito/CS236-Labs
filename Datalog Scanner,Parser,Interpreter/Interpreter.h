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
	vector<int> columnsNumbersToProject;
	vector<string> queryValues;
	vector<string> variableNamesAdded;

	// Helper functions

	//Database creation
	void createAndAddTables();
	void createAndAddRows();
	Relation createTable(string name, vector<string> scheme);
	Scheme createColumnHeaders(vector<string> columnHeaders);
	Tuple createRow(vector<string> rowValues);

	// Query evalution
	void evaluateQueries();
	void evaluateQuery(string queryScheme);
	void addToColumnNumsToProject(int columnNumToAdd);
	void addToColumnNumsToProject(string variableName, int columnNumToAdd);
	void performConstantSelects(Relation& relationCopy);
	void performVariableSelects(Relation& relationCopy);
	void performProjectionAndRenaming(Relation& relationCopy);
	bool queryHasVariables();

	// Rule evaluation
	void evaluteRules();
	void evaluteRule(string ruleHeadName, vector<string> ruleHeadValues, vector<string> ruleNames, vector<vector<string>> ruleValues);
	bool numOfDBRowsHasChanged(int oldNumOfRows);
	vector<Relation> performEvalOfRightSideRules(vector<string>& ruleNames, vector<vector<string>>& ruleValues);
	void performJoins(Relation& relationCopy, vector<Relation>& rightSideRelations);
	void getRuleColumnsToProject(vector<string>& ruleHeadVals, vector<string>& endRelationSchemeVals);

	// Output
	string toStringQueryEvaluation(Relation& finalRelation, string querySchemeName);
	string toStringQuery(string querySchemeName);
	string toStringQueryWithVariables(set<Tuple>& rows);

	string toStringRule(string& ruleHeadName, vector<string>& ruleHeadValues, vector<string>& ruleNames, vector<vector<string>>& ruleValues) const;
	string toStringHeadRule(string& ruleHeadName, vector<string>& ruleHeadValues) const;
	string toStringRulePredicates(vector<string>& ruleNames, vector<vector<string>>& ruleValues) const;

};
