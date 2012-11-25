#ifndef _ASTAR_H_
#define _ASTAR_H_

#ifdef WIN32
#include <math.h>
#else
#include <fastmath.h>
#endif

#include <vector>
#include <algorithm>
#include "Node.h"
#include "TileMap.h"
#include "Person.h"
#include "Collision.h"
#include "Grid.h"

struct SortByF
{
	bool operator()(Node* first, Node* second)
	{ 
		if (first->F > second->F) {
			return true;
		}
		else if (first->F == second->F) {
			if (first->H > second->H) {
				return true;
			}
		}
		return false;
	}
};
//------------------------------------------------------------------------------------------------
class AStar
{
private:
	std::vector<Node*> mNodes;

	int GetG(Node* node, Node* parent, int randomfactor);
	int GetHeuristic(Node* start, Node* end);

	//TileMap* mMap;

protected:

public:

	AStar();
	~AStar();

	std::vector<Node*> GetPath(Node* start, Node* end, int randomfactor = 1);

	Node* GetClosestNode(Person* person);

	void Rebuild(std::vector<Node*>* nodes, Grid* mGrid);

};
#endif
