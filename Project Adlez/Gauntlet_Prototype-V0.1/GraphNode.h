#pragma once
#include "HelpfulData.h"

struct Connection
{
	GraphNode* neighbor;
	float cost;
};

class GraphNode
{
private:
	TemplateVector<Connection> m_neighbors;
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