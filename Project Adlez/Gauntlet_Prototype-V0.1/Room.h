#pragma once
#include "HelpfulData.h"
#include "Player.h"
#include "Texture.h"
#include "Graph.h"
extern class GameEngine;
// room dimensions
#define ROOM_WIDTH 17
#define ROOM_HEIGHT 13
// door values
#define NUM_DOORS 4
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

class Room;

// types of nodes in floors
enum NODE_TYPE {WALL, DOOR, FLOOR, BAD};

// used internally to denote how the room should be generated and treated
enum ROOM_TYPE {BASIC, START, END, OUTSIDE};

struct Door
{
	Room* nextRoom;
	bool locked;
	V2DF posNew;
};

struct Node
{
	Door m_door;
	NODE_TYPE m_type;
	int m_graphNodeID;
};

class Room
{
private:
	// reference to game engine
	GameEngine* pGEngine;
	// Graph of the room used for pathfinding
	Graph m_graph;
	// layout of the room in nodes
	Node m_layout[ROOM_HEIGHT][ROOM_WIDTH];
	// textures used by the room's various node types
	Texture m_floorTex;
	Texture m_wallBaseTex;
	Texture m_edgeTex;
	Texture m_connectTex;
	Texture m_doorTex;
	Texture m_lockedTex;
	// room control variables
	bool visited;
	ROOM_TYPE m_type;
public:
	Room();
	~Room();
	void initialize(char* roomFile, ROOM_TYPE type, GameEngine *ref);
	void release();
	void render();
	void setDoors(Room* doors[4]);
	Node checkLocation(int x, int y);
	GraphNode* getNode(int x, int y);
	void enterRoom(Player * thePlayer);
	void update(float dT);
	bool coll_walls(Entity* entity);
	Room* collOpenDoors(FRect a_rect, V2DF& posNew);
};