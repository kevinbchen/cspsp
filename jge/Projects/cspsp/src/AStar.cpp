#include "AStar.h"

//------------------------------------------------------------------------------------------------
AStar::AStar()
{

}

//------------------------------------------------------------------------------------------------
AStar::~AStar()
{
	mNodes.clear();
}

//------------------------------------------------------------------------------------------------
std::vector<Node*> AStar::GetPath(Node* start, Node* end, int randomfactor)
{
	for (unsigned int i=0; i<mNodes.size(); i++) { 
		mNodes[i]->mOpenList = false;
		mNodes[i]->mClosedList = false;
	}

	std::vector<Node*> openlist;
	std::vector<Node*> closedlist;

	start->mParent = start;
	start->G = 0;
	start->H = GetHeuristic(start,end);
	start->F = start->G + start->H;
	start->mOpenList = true;
	openlist.push_back(start);
	
	bool exists = true;
	Node* currentnode;
	while(true) {
		std::sort(openlist.begin(),openlist.end(),SortByF());
		currentnode = openlist.back();

		if (currentnode == end) break; //found end
		if (openlist.size() == 0) {
			exists = false;
			break;
		}

		openlist.pop_back();
		closedlist.push_back(currentnode);
		currentnode->mOpenList = false;
		currentnode->mClosedList = true;

		for (unsigned int i=0; i<currentnode->mConnections.size(); i++) {
			if (currentnode->mConnections[i]->mClosedList == true) continue;

			if (currentnode->mConnections[i]->mOpenList == false) {
				openlist.push_back(currentnode->mConnections[i]);
				currentnode->mConnections[i]->mOpenList = true;
				currentnode->mConnections[i]->mParent = currentnode;
				currentnode->mConnections[i]->G = GetG(currentnode->mConnections[i],currentnode->mConnections[i]->mParent,randomfactor);
				currentnode->mConnections[i]->H = GetHeuristic(currentnode->mConnections[i],end);
				currentnode->mConnections[i]->F = currentnode->mConnections[i]->G + currentnode->mConnections[i]->H;
			}
			else {
				int tempG = GetG(currentnode->mConnections[i],currentnode,randomfactor);
				if (tempG < currentnode->mConnections[i]->G) {
					currentnode->mConnections[i]->mParent = currentnode;
					currentnode->mConnections[i]->G = tempG;
					currentnode->mConnections[i]->F = currentnode->mConnections[i]->G + currentnode->mConnections[i]->H;
				}
			}
		}
	}

	std::vector<Node*> path;

	if (exists) {
		Node* tempnode = end;
		while (true) {
			path.push_back(tempnode);

			if (tempnode == start) break;

			tempnode = tempnode->mParent;
			if (tempnode->mParent == NULL) break;
		}
	}
	else {
		path.push_back(start);
	}
	return path;
}

//------------------------------------------------------------------------------------------------
Node* AStar::GetClosestNode(Person* person)
{
	Node* node = NULL;
	float distance = -1.0f;
	for (unsigned int i=0; i<mNodes.size(); i++) {
		float dx = mNodes[i]->mX-person->mX;
		float dy = mNodes[i]->mY-person->mY;
		float tempdistance = dx*dx+dy*dy;

		if (tempdistance < distance || distance < 0.0f) {
			/*Line line1(mNodes[i]->mX,mNodes[i]->mY,person->mX,person->mY);

			bool isConnected = true;
			for (unsigned int j=0; j<mMap->mCollisionLines.size(); j++) {
				if (mMap->mCollisionLines[j].people == false) continue;

				Line line2 = mMap->mCollisionLines[j].line;

				Vector2D d;
				if (LineLineIntersect(line1,line2,d)) {
					isConnected = false;
					break;
				}
			}
			if (isConnected) {*/
				distance = tempdistance;
				node = mNodes[i];
			//}
		}
	}
	return node;

}

//------------------------------------------------------------------------------------------------
void AStar::Rebuild(std::vector<Node*>* nodes, Grid* mGrid)
{
	//mMap = map;

	mNodes = *nodes;

	for (unsigned int i=0; i<mNodes.size(); i++) { 
		for (unsigned int j=0; j<mNodes.size(); j++) {
			if (mNodes[i] == mNodes[j]) continue;
			bool isConnected = true;
			isConnected = mGrid->LineOfSight(mNodes[i]->mX,mNodes[i]->mY,mNodes[j]->mX,mNodes[j]->mY,true,true);

			/*Vector2D A(mNodes[i]->mX,mNodes[i]->mY);
			Vector2D B(mNodes[j]->mX,mNodes[j]->mY);
			Line line1(A,B);
			for (int k=0;k<mMap->mCollisionLines.size();k++) {
				if (mMap->mCollisionLines[k].people == false) continue;

				Line line2 = mMap->mCollisionLines[k].line;

				Vector2D d;
				if (LineLineIntersect(line1,line2,d)) {
					isConnected = false;
					break;
				}
			}*/
			if (isConnected) {
				mNodes[i]->mConnections.push_back(mNodes[j]);
			}
		}
	} 
}


//------------------------------------------------------------------------------------------------
int AStar::GetG(Node* node, Node* parent, int randomfactor)
{
	int dx = node->mX-parent->mX;
	int dy = node->mY-parent->mY;
	return (int)sqrtf(dx*dx + dy*dy) + rand()%randomfactor;
}

//------------------------------------------------------------------------------------------------
int AStar::GetHeuristic(Node* start, Node* end)
{
	int dx = abs(start->mX-end->mX);
	int dy = abs(start->mY-end->mY);
	return dx+dy;
}
