#pragma once

#include "Node.h"
#include <map>
#include <stack>
#include <vector>
#include <iostream>

using namespace std;

class Graph
{
public:
	Graph();
	~Graph();

	map<int, Node> getNodes();
	void setNodes(map<int, Node> nodes);
	bool addNode(int ruleIndex);
	void pushPostOrderNum(int num);
	int popPostOrderNum();
	stack<int> getPostOrderNums();
	void setPostOrderNums(stack<int> order);
	void addEdge(int currentNode, int nodeToConnect);

	vector<set<int>> findSCCs();
	void dfsForest();
	void dfs(Node& node, bool changePostOrderNum); // Depth First Search

	string toString();

private:
	map<int, Node> nodes;
	stack<int> postOrderNums;
	set<int> scc; 			
	vector<set<int>> sccs; // Strongly Connected Components
};

