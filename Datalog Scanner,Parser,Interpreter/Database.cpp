#include "Database.h"

Database::Database() {
}

Database::~Database() {
}

void Database::addTable(string name, Relation table) {
	this->tables.insert({name, table});
}

void Database::addRowToTable(string relationName, Tuple tupleToAdd) {
	this->tables[relationName].addRow(tupleToAdd);
}

void Database::updateTable(string tableName, Relation newTable) {
	this->tables[tableName] = newTable;
}

Relation Database::findTable(string relationName) {
	return this->tables[relationName];
}

int Database::getNumTables() {
	return this->tables.size();
}

int Database::getTotalNumRows() {
	int numRows = 0;
	map<string, Relation>::iterator iter = this->tables.begin();
	map<string, Relation>::iterator end = this->tables.end();

	for (; iter != end; iter++) {
		numRows += iter->second.getNumOfRows();
	}

	return numRows;
}

string Database::toString() {
	ostringstream oss;
	map<string, Relation>::iterator iter = this->tables.begin();
	map<string, Relation>::iterator last = this->tables.end();
	for (; iter != last; ++iter) {
		oss << "\t" << iter->second.toString() << endl;
	}
	return oss.str();
}
