#include "Graph.h"

Graph::Graph() {
	this->nodes = {};
	this->scc = {};
	this->sccs = {};
}

Graph::~Graph() {
}

map<int, Node> Graph::getNodes() {
	return this->nodes;
}

void Graph::setNodes(map<int, Node> nodes) {
	this->nodes = nodes;
}

bool Graph::addNode(int ruleIndex) {
	Node newNode = Node(ruleIndex);

	if (this->nodes.insert({ ruleIndex, newNode }).second) {
		return true;
	}
	else { // Duplicate or invalid value
		return false;
	}
}

void Graph::pushPostOrderNum(int num) {
	this->postOrderNums.push(num);
}

int Graph::popPostOrderNum() {
	int top = this->postOrderNums.top();
	this->postOrderNums.pop();
	return top;
}

stack<int> Graph::getPostOrderNums() {
	return this->postOrderNums;
}

void Graph::setPostOrderNums(stack<int> order) {
	this->postOrderNums = order;
}

void Graph::addEdge(int currentNode, int nodeToConnect) {
	this->nodes.find(currentNode)->second.addAdjacentNode(nodeToConnect);
}

vector<set<int>> Graph::findSCCs() {
	//cout << "FINDING SCCS" << endl;
	while (this->postOrderNums.size() > 0) {
		int nodeIndex = this->popPostOrderNum();
		//cout << "POPPING OFF R" << nodeIndex << endl;
		this->scc = {}; // Reset this scc container for every new iteration

		if (this->nodes.at(nodeIndex).getVisited() == false) {
			this->dfs(this->nodes.at(nodeIndex), false);
			this->sccs.push_back(this->scc);
		}
	}

	return this->sccs;
}

void Graph::dfsForest() {
	for (auto& iter : this->nodes) {
		if (iter.second.getVisited() == false) {
			this->dfs(iter.second, true);
		}
	}

}

void Graph::dfs(Node& node, bool changePostOrderNums) {
	node.setVisited(true);

	for (int nodeIndex : node.getIndexesOfAdjacentNodes()) {
		if (this->nodes.at(nodeIndex).getVisited() == false) {
			if (changePostOrderNums) {
				this->dfs(this->nodes.at(nodeIndex), true);
			}
			else {
				this->dfs(this->nodes.at(nodeIndex), false);
			}
		}
	}

	if (changePostOrderNums) {
		this->pushPostOrderNum(node.getNodeNum());
	}
	this->scc.insert(node.getNodeNum());

}


string Graph::toString() {
	ostringstream oss;
	map<int, Node>::iterator iter = this->nodes.begin();
	map<int, Node>::iterator last = this->nodes.end();

	for (; iter != last; iter++) {
		oss << "R" << iter->first << ":" << iter->second.toStringAdjacentNodes() << endl;
	}

	return oss.str();
}
