#ifndef _NODE_H_
#define _NODE_H_

#include <vector>

//------------------------------------------------------------------------------------------------

class Node
{
private:

protected:

public:
	int mX;
	int mY;

	int mNumConnections;
	std::vector<Node*> mConnections;


	//for pathfinding
	Node* mParent;
	int F;
	int G;
	int H;
	bool mOpenList;
	bool mClosedList;

	Node(int x, int y);
	~Node();

};
#endif
