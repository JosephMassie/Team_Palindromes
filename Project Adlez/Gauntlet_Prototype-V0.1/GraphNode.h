#pragma once
#include "HelpfulData.h"

class GraphNode;

// used to easily store connections
struct Connection
{
	GraphNode* neighbor;
	float cost;
};

class GraphNode
{
private:
	// all connections this node has
	TemplateVector<Connection> m_neighbors;
	// position in world space
	V2DF m_position;
public:
	GraphNode();
	~GraphNode();
	void release();
	void initialize(V2DF a_pos);
	void addNeighbor(GraphNode* a_neighbor, float a_cost);
	TemplateVector<Connection>* getNeighbors();
	V2DF getPosition();
};