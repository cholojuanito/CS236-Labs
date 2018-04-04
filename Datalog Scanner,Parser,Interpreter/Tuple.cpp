#include "Tuple.h"

Tuple::Tuple() {
}

Tuple::~Tuple() {
}

string Tuple::toString() const {
	ostringstream oss;
	int numItems = this->size();

	for (int i = 0; i < numItems; i++) {
		oss << this->at(i) << "\t";
	}

	return oss.str();
}

