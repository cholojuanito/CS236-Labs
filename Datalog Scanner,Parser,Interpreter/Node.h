#pragma once

#include <set>

using namespace std;

class Node
{
public:
	Node();
	~Node();
	bool getVisited();
	void setVisited(bool visited);
	int getPostOrderNum();
	void setPostOrderNum(int postOrderNum);
	set<int> getIndexesOfAdjacentNodes();
	void setIndexesOfAdjacentNodes(set<int> indexes);
	bool addAdjacentNode(int index);

private:
	set<int> indexesOfAdjacentNodes;
	bool visited;
	int postOrderNum;
};

