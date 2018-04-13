#pragma once

#include <set>
#include <sstream>

using namespace std;

class Node
{
public:
	Node();
	Node(int nodeNum);
	~Node();
	//string getNodeValue();
	//void setNodeValue(string value);
	int getNodeNum();
	void setNodeNum(int num);
	bool getVisited();
	void setVisited(bool visited);
	int getPostOrderNum();
	void setPostOrderNum(int postOrderNum);
	set<int> getIndexesOfAdjacentNodes();
	void setIndexesOfAdjacentNodes(set<int> indexes);

	bool addAdjacentNode(int index);

	string toStringAdjacentNodes();
	string toString();
	
private:
	//string nodeValue;
	int nodeNum;
	set<int> indexesOfAdjacentNodes;
	bool visited;
	int postOrderNum;
};

