#include "Node.h"



Node::Node()
{
}


Node::~Node()
{
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
