#pragma once
#include "HelpfulData.h"
#include "GraphNode.h"

class Graph
{
private:
	bool **m_AdjacencyMatrix;
	TemplateVector<GraphNode*> m_nodes;
public:
	Graph();

};