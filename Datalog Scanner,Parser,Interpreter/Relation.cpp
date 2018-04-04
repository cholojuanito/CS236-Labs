#include "Relation.h"

Relation::Relation() {
	this->name = "DEFAULT";
	this->columnHeaders = {};
	this->rows = {};
}

Relation::Relation(string name, Scheme scheme) {
	this->name = name;
	this->columnHeaders = scheme;
}

Relation::~Relation() {
}

string Relation::getName() const {
	return this->name;
}

Scheme Relation::getScheme() const {
	return this->columnHeaders;
}

set<Tuple> Relation::getRows() const {
	return this->rows;
}

int Relation::getNumOfRows() const {
	return this->rows.size();
}

void Relation::setName(string name) {
	this->name = name;
}

void Relation::setScheme(Scheme scheme) {
	this->columnHeaders = scheme;
}

void Relation::setRows(set<Tuple> rows) {
	this->rows = rows;
}

Scheme Relation::createScheme(vector<string> names) {
	return Scheme();
}

Scheme Relation::changeScheme(Scheme newScheme) {
	this->setScheme(newScheme);
	return this->columnHeaders;
}

bool Relation::addRow(Tuple tuple) {
	if (this->rows.insert(tuple).second) {
		return true; // If the row was added to the table
	}
	else {
		return false; // New row was not added or was a duplicate
	}
}

/*
 * This select function allows for a string value and
 * the column that value should be in.
 * If the parameter value matches the value in the column
 * a new Relation with those Tuples is returned.
 */
Relation Relation::select(string value, int column) {
	Relation newRelation = Relation(this->name, this->columnHeaders);
	set<Tuple>::iterator iter = this->rows.begin();
	set<Tuple>::iterator last = this->rows.end();

	for (; iter != last; iter++) {
		string current = iter->at(column);
		if (current == value) {
			newRelation.addRow(*iter);
		}
	}

	return newRelation;
}

/*
 * This select function allows for two columns.
 * If the values in both positions are the same
 * a new Relation with the same Scheme and name
 * but only specific Tuples is returned.
 */
Relation Relation::select(int column1, int column2) {
	Relation newRelation = Relation(this->name, this->columnHeaders);
	set<Tuple>::iterator iter = this->rows.begin();
	set<Tuple>::iterator last = this->rows.end();

	for (; iter != last; iter++) {
		string firstVal = iter->at(column1);
		string secondVal = iter->at(column2);
		if (firstVal == secondVal) {
			newRelation.addRow(*iter);
		}
	}

	return newRelation;
}

/*
 * Creates a new Relation with the specifed number of columns
 * and the values in those columns from the Relation
 * that was called.
 */
Relation Relation::project(vector<int> columns) {
	Relation newRelation = Relation();
	newRelation.setName(this->name);
	vector<string> oldSchemeValues = this->columnHeaders.getColumnHeaders();
	vector<string> newSchemeValues = {};
	Scheme newScheme;
	int numNewColumns = columns.size();
	set<Tuple>::iterator iter = this->rows.begin();
	set<Tuple>::iterator last = this->rows.end();

	// Find the values for the new scheme
	for (int i = 0; i < numNewColumns; i++) {
		int newColumnIndex = columns.at(i);
		string columnName = oldSchemeValues.at(newColumnIndex);
		newSchemeValues.push_back(columnName);
	}
	// Find the table values in the corresponding columns that need to be copied
	for (; iter != last; iter++) {
		Tuple newRowValue = {};
		for (int i = 0; i < numNewColumns; i++) {
			int newColumnIndex = columns.at(i);
			newRowValue.push_back(iter->at(newColumnIndex));
		}

		newRelation.addRow(newRowValue);
	}

	newScheme.setColumnHeaders(newSchemeValues);
	newRelation.changeScheme(newScheme);

	return newRelation;
}

Relation Relation::rename(Scheme newScheme) {
	Relation newRelation = Relation(this->name, newScheme);
	newRelation.setRows(this->rows);
	return newRelation;
}

/*
 * Join function
 */
Relation Relation::join(Relation& r1, Relation& r2) {
	// TODO: Change to pass by reference
	Relation newRelation = Relation();
	Scheme s1 = r1.getScheme();
	Scheme s2 = r2.getScheme();
	set<Tuple> t1 = r1.getRows();
	set<Tuple> t2 = r2.getRows();
	set<Tuple>::iterator iterT1, iterT2;
	set<Tuple>::iterator lastT1 = t1.end();
	set<Tuple>::iterator lastT2 = t2.end();

	Scheme newScheme = this->joinScheme(s1, s2);
	newRelation.changeScheme(newScheme);

	for (iterT1 = t1.begin(); iterT1 != lastT1; iterT1++) {
		Tuple row1 = *iterT1;
		for (iterT2 = t2.begin(); iterT2 != lastT2; iterT2++) {
			Tuple row2 = *iterT2;
			if (this->rowsAreJoinable(row1, row2, s1, s2)) {
				 Tuple newRow = this->joinRows(row1, row2, s1, s2);

				 newRelation.addRow(newRow);
			}
		}
	}

	return newRelation;
}

/*
 * Union function
 * Take rows from resultOfRuleEval and add to rows of the table in the database.
 * Make sure it is persisted into the database
 */
void Relation::onion(Relation& resultOfRuleEval) {
	for (auto row : resultOfRuleEval.rows) {
		if (this->addRow(row)) {
			// Make a toString of the row added according to the output wanted
			cout << this->toStringNewRow(row);
		}
	}
}

string Relation::toString() const {
	ostringstream oss;
	set<Tuple>::iterator iter = this->rows.begin();
	set<Tuple>::iterator last = this->rows.end();

	oss << endl << "Table: " << this->name << endl;
	oss << "\t" << this->columnHeaders.toString() << endl;
	for (; iter != last; iter++) {
		oss << "\t" << iter->toString() << endl;
	}

	return oss.str();
}

string Relation::toStringNewRow(Tuple& newRow) const {
	ostringstream oss;
	int numColumns = newRow.size();
	vector<string> colHeaders = this->columnHeaders.getColumnHeaders();
	string twoSpaces = "  "; // "identation" for each row

	oss << twoSpaces;
	for (int i = 0; i < numColumns; i++) {
		if (i < numColumns - 1) {
			oss << colHeaders.at(i) << "=" << newRow.at(i) << ", ";
		}
		else {
			oss << colHeaders.at(i) << "=" << newRow.at(i);
		}
	}
	oss << endl;

	return oss.str();
}

Scheme Relation::joinScheme(Scheme& s1, Scheme& s2) const {
	vector<string> s1Headers = s1.getColumnHeaders();
	vector<string> s2Headers = s2.getColumnHeaders();
	vector<string> newHeaders = s1Headers;
	int s1NumColumns = s1.getNumColumns();
	int s2NumColumns = s2.getNumColumns();

	// Go through the second relations column headers and find the unique ones to add.
	for (int i = 0; i < s2NumColumns; i++) {
		bool isUniqueColumn = true;
		string headerVal = s2Headers.at(i);
		for (int j = 0; j < s1NumColumns; j++) {
			if (s1Headers.at(j) == headerVal) {
				isUniqueColumn = false;
			}
		}
		if (isUniqueColumn) {
			newHeaders.push_back(headerVal);
		}
	}

	Scheme newScheme = Scheme(newHeaders);

	return newScheme;
}

Tuple Relation::joinRows(Tuple& t1, Tuple& t2, Scheme& s1, Scheme& s2) const {
	/*
	 Same as joinable function but check if the columns are different names.
	*/
/*
	cout << "ROW 1: ";
	for (int i = 0; i < t1.size(); i++) {
		cout << t1.at(i) << " ";
	}
	cout << "\nROW 2: ";
	for (int i = 0; i < t2.size(); i++) {
		cout << t2.at(i) << " ";
	}
	cout << endl;
*/

	vector<string> s1Headers = s1.getColumnHeaders();
	vector<string> s2Headers = s2.getColumnHeaders();

	Tuple newTuple = t1;
	int row2Size = t2.size();
	for (int i = 0; i < row2Size; i++) {
		string value = t2.at(i);
		string s2ColumnName = s2Headers.at(i);

		if (this->isColumnNameDiff(s1Headers, s2ColumnName)) {
			newTuple.push_back(value);
		}

	}

	return newTuple;
}

bool Relation::rowsAreJoinable(Tuple& t1, Tuple& t2, Scheme& s1, Scheme& s2) const {
	vector<string> s1Headers = s1.getColumnHeaders();
	vector<string> s2Headers = s2.getColumnHeaders();
	int s1NumColumns = s1.getNumColumns();
	int s2NumColumns = s2.getNumColumns();

	for (int i = 0; i < s1NumColumns; i++) {
		string s1Value = s1Headers.at(i);
		string t1Value = t1.at(i);
		for (int j = 0; j < s2NumColumns; j++) {
			string s2Value = s2Headers.at(j);
			string t2Value = t2.at(j);
			// Check if the column headers are the same
			if (s1Value == s2Value) {
				// If they are then check the values in those positions.
				// If they are NOT the same then the rows are not joinable.
				if (t1Value != t2Value) {
					/*cout << endl << "THERE WAS AN ISSUE WITH 1's VALUE " << t1Value << "OF COLUMN " <<  endl
						<< "AND 2's VALUE" << t2Value << "OF COLUMN " << s2Value << "!" << endl;*/
					return false;
				}
			}
		}

	}

	return true;
}

bool Relation::isColumnNameDiff(vector<string>& col1Names, string col2Name) const {
	int row1Size = col1Names.size();
	for (int i = 0; i < row1Size; i++) {
		if (col1Names.at(i) == col2Name) {
			return false;
		}
	}
	return true;
}
