#include "GraphNode.h"

GraphNode::GraphNode() {}

// make sure the safer release is called
GraphNode::~GraphNode() {release();}

void GraphNode::release()
{
	m_neighbors.release();
}

// setup node at given position
void GraphNode::initialize(V2DF a_pos)
{
	m_position = a_pos;
}

// create a connection with the given neighbor and add it to the list
void GraphNode::addNeighbor(GraphNode* a_neighbor, float a_cost)
{
	Connection temp;
	temp.neighbor = a_neighbor;
	temp.cost = a_cost;
	m_neighbors.add(temp);
}

TemplateVector<Connection>* GraphNode::getNeighbors()
{
	return &m_neighbors;
}

V2DF GraphNode::getPosition()
{
	return m_position;
}

// returns the hueristic value between the given node and this node
float GraphNode::Hueristic(GraphNode* other)
{
	// make sure other exists
	if(!other)
		return 0.0f;
	// calculate hueristic
	return other->getPosition().difference( m_position ).length();
}