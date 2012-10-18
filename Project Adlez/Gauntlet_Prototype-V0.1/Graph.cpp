#include "Graph.h"

// default constructor
Graph::Graph() {}

// default destructor
// :make sure safer release is called
Graph::~Graph() { release(); }

// safetly release all data
void Graph::release()
{
	// release the adjacency matrix
	for(int i = 0; i < m_size; ++i)
	{
		delete [] m_AdjacencyMatrix[i];
	}
	delete [] m_AdjacencyMatrix;

	// release the graphNodes
	for(int i = 0; i < m_nodes.size(); ++i)
	{
		m_nodes.get(i)->release();
	}

	// now release the list of nodes itself
	m_nodes.release();

	// release the debug texture
	m_nodeTex.release();
	m_neighborTex.release();
}

// set up the graph to be the given size
void Graph::initialize(int a_size)
{
	// setup debug texture
	m_nodeTex.initialize(L"images/graph.png");
	m_neighborTex.initialize(L"images/neighbor.png");

	// setup graph to be the given size
	m_size = a_size;

	// create the adjaceny matrix
	m_AdjacencyMatrix = new bool*[m_size];
	for(int i = 0; i < m_size; i++)
		m_AdjacencyMatrix[i] = new bool[m_size];

	// initialize the entire matrix to false
	for(int y = 0; y < m_size; y++)
		for(int x = 0; x < m_size; x++)
			m_AdjacencyMatrix[y][x] = false;
}

// create a new node with the given world position
// the node's id is returned
// if -1 is returned an error occured
// most likely cause is the graph is full
int Graph::createNode(V2DF a_pos)
{
	// first check if the graph is to big
	if( m_nodes.size() >= m_size )
		return -1;

	// if the graph has room add the node
	GraphNode* temp = new GraphNode();
	temp->initialize(a_pos);
	m_nodes.add(temp);

	// return this new nodes id
	return m_nodes.size() - 1;
}

// return a reference to the given graph node
// return NULL if the node doesn't exist
GraphNode* Graph::getNode(int a_node)
{
	// make sure node exists
	if(a_node >= 0 && a_node < m_nodes.size())
	{
		// return a reference to the node
		return m_nodes.get(a_node);
	}

	// node doesn't exist return NULL
	return 0;
}

// creates a one way connection between the two nodes going from start node to end node with the given cost
void Graph::setOneWayConnection(int a_start, int a_end, int a_cost)
{
	// make sure the nodes exist
	if( a_start <= -1 || a_start >= m_size || a_end <= -1 || a_end >= m_size 
		|| a_start >= m_nodes.size() || a_end >= m_nodes.size() )
		return;

	// the nodes exist so create the connection
	// first update the adjacency matrix
	m_AdjacencyMatrix[a_start][a_end] = true;
	
	// second update the start node
	m_nodes.get(a_start)->addNeighbor( m_nodes.get(a_end), a_cost );
}

// create a two way connection between nodes start and end each with their own cost
// costA = start -> end
// costB = end -> start
void Graph::setTwoWayConnection(int a_start, int a_end, int costA, int costB)
{
	// set first connection
	setOneWayConnection(a_start, a_end, costA);
	// set second connection
	setOneWayConnection(a_end, a_start, costB);
}

// create a two way connection between nodes start and end with the same cost
void Graph::setTwoWayConnection(int a_start, int a_end, int a_cost)
{
	// set first connection
	setOneWayConnection(a_start, a_end, a_cost);
	// set second connection
	setOneWayConnection(a_end, a_start, a_cost);
}

// Used for debugging
void Graph::render()
{
	for(int i = 0; i < m_nodes.size(); ++i)
	{
		V2DF parent = m_nodes.get(i)->getPosition();
		V2DF dif(0.0f,0.0f);
		V2DF child(0.0f,0.0f);
		m_nodeTex.draw( parent, 0.0f, 1.0f );
		// draw its neighbors
		float angle = 0.0f;
		TemplateVector<Connection>* m_neighbors = m_nodes.get(i)->getNeighbors();
		for(int j = 0; j < m_neighbors->size(); ++j)
		{
			child = m_neighbors->get(j).neighbor->getPosition();
			dif = child.difference(parent);
			angle = atan2f(dif.y,dif.x)*(180.0f/V2D_PI) + 90.0f;
			m_neighborTex.draw( parent, angle, 1.0f );
		}
	}
}