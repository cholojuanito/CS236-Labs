#pragma once

/*
 * Tanner Davis March 1st 2018
 * Tuple.h
 * 
 * A tuple represents the values in a row of the database.
 * 
 */
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class Tuple : public vector<string>
{
public:
	Tuple();
	~Tuple();
	string toString() const;

};

