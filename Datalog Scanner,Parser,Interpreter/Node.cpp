#include "Node.h"



Node::Node() {
	this->indexesOfAdjacentNodes = {};
	this->nodeNum = -1;
	this->postOrderNum = 0;
	this->visited = false;
}

Node::Node(int nodeNum) {
	this->indexesOfAdjacentNodes = {};
	this->nodeNum = nodeNum;
	this->postOrderNum = 0;
	this->visited = false;
}


Node::~Node() {
}

int Node::getNodeNum() {
	return this->nodeNum;
}

void Node::setNodeNum(int num) {
	this->nodeNum = num;
}

bool Node::getVisited() {
	return this->visited;
}

void Node::setVisited(bool visited) {
	this->visited = visited;
}

int Node::getPostOrderNum() {
	return this->postOrderNum;
}

void Node::setPostOrderNum(int postOrderNum) {
	this->postOrderNum = postOrderNum;
}

set<int> Node::getIndexesOfAdjacentNodes() {
	return this->indexesOfAdjacentNodes;
}

void Node::setIndexesOfAdjacentNodes(set<int> indexes) {
	this->indexesOfAdjacentNodes = indexes;
}

bool Node::addAdjacentNode(int index) {
	if (this->indexesOfAdjacentNodes.insert(index).second) {
		return true;
	}
	else { // Duplicate or invalid value
		return false;
	}
}

string Node::toStringAdjacentNodes() {
	ostringstream oss;
	set<int>::iterator iter = this->indexesOfAdjacentNodes.begin();
	set<int>::iterator last = this->indexesOfAdjacentNodes.end();

	for (; iter != last; iter++) {
		if (next(iter) == last) {
			oss << "R" << *iter;
		}
		else {
			oss << "R" << *iter << ",";
		}
	}

	return oss.str();
}

string Node::toString() {
	ostringstream oss;
	oss << "R" << this->nodeNum << endl;
	oss << "VISITED: ";
	if (this->visited) {
		oss << "YES" << endl;
	}
	else {
		oss << "NO" << endl;
	}
	oss << "POST ORDER NUM: " << this->postOrderNum << endl;

	return oss.str();
}
