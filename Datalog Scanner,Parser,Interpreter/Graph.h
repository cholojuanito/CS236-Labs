#pragma once

#include "Node.h"
#include <map>

class Graph
{
public:
	Graph();
	~Graph();

	map<int, Node> getNodes();
	void setNodes(map<int, Node> nodes);
	bool addNode(int ruleIndex, Node node);

private:
	map<int, Node> nodes;
};

