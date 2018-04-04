#include "Scheme.h"

Scheme::Scheme() {
}

Scheme::Scheme(vector<string> names) {
	this->columnHeaders = names;
}

Scheme::~Scheme() {
}

vector<string> Scheme::getColumnHeaders() const {
	return this->columnHeaders;
}

void Scheme::setColumnHeaders(vector<string> names) {
	this->columnHeaders = names;
}

int Scheme::getNumColumns() {
	return this->columnHeaders.size();
}

string Scheme::toString() const {
	ostringstream oss;
	int numItems = this->columnHeaders.size();

	for (int i = 0; i < numItems; i++) {
		oss << this->columnHeaders.at(i) << "\t";
	}
	return oss.str();
}
