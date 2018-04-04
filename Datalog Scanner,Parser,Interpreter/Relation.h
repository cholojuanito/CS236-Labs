#pragma once

/*
* Tanner Davis March 1st 2018
* Tuple.h
*
* A relation represents a database table.  
* Each relation has:
*		1) A name, to identify it
*		2) A scheme, to label each column with and differentiate from other relations
*		3) A set of tuples, to represent the values of each row in the database
*/

#include "Tuple.h"
#include "Scheme.h"
#include <set>

class Relation
{
public:
	Relation();
	Relation(string name, Scheme scheme);
	~Relation();

	string getName() const;
	Scheme getScheme() const;
	set<Tuple> getRows() const;
	int getNumOfRows() const;
	void setName(string name);
	void setScheme(Scheme scheme);
	void setRows(set<Tuple> rows);

	// Database creation
	Scheme createScheme(vector<string> headers);
	Scheme changeScheme(Scheme newScheme);
	bool addRow(Tuple row);

	// Query evaluation
	Relation select(string value, int column);
	Relation select(int column1, int column);
	Relation project(vector<int> columns);
	Relation rename(Scheme newScheme);

	// Rule evaluation
	Relation join(Relation& r1, Relation& r2);
	void onion(Relation& resultOfRuleEval); // Supposed to be union but you wankers took that keyword.

	string toString() const;
	string toStringNewRow(Tuple& newRow) const;

protected:
	string name;			// The table's name so that it can be identified
	Scheme columnHeaders;	// Represents the columns in the table
	set<Tuple> rows;		// Represent the different rows and their specific values in each column

private:

	// Rule evaluation
	Scheme joinScheme(Scheme& r1, Scheme& r2) const;
	Tuple joinRows(Tuple& t1, Tuple& t2, Scheme& s1, Scheme& s2) const;
	bool rowsAreJoinable(Tuple& t1, Tuple& t2, Scheme& s1, Scheme& s2) const;
	bool isColumnNameDiff(vector<string>& col1Names, string col2Name) const;

};

