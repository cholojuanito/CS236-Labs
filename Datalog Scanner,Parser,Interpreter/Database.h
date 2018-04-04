#pragma once
/*
* Tanner Davis March 1st 2018
* Database.h
*
* A tuple represents the values in a row of the database.
*
*/

#include "Relation.h"
#include <map>

class Database
{
public:
	Database();
	~Database();

	void addTable(string name, Relation relation);
	void addRowToTable(string relationName, Tuple tupleToAdd);
	void updateTable(string tableName, Relation newTable);
	Relation findTable(string relationName);
	int getNumTables();
	int getTotalNumRows();

	string toString();
	//string toString() const;

protected:
	map<string, Relation> tables; // Each table has a name to identify it.
};

