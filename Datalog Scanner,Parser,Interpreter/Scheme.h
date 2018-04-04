#pragma once
/*
* Tanner Davis March 1st 2018
* Scheme.h
*
* A scheme represents the column names in the database.
*
*/

#include <vector>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

class Scheme
{
public:
	Scheme();
	Scheme(vector<string> columnHeaders);
	~Scheme();

	vector<string> getColumnHeaders() const;
	void setColumnHeaders(vector<string> names);
	int getNumColumns();
	string toString() const;

protected:
	vector<string> columnHeaders;
};

