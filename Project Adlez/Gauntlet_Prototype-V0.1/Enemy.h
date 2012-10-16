#pragma once
#include "Entity.h"
#include "astar.h"
extern class Player;
extern class Room;

enum ENEMY_TYPE {GHOST, BOSS};

class Enemy : public Entity
{
private:
	// enemy stat  line
	float atkRange;
	float siteRange;
	ClDwn atkTimer;
	ClDwn aniTimer;
	ClDwn ultTimer;
	float maxHealth;
	float curHeatlh;
	float mSpd;
	float dmg;
	float aSpd;
	
	// this will define the enemies stat line and texture
	ENEMY_TYPE m_type;
	V2DF steeringForce;
	Astar pathFinder;
	float wanderAngle;
	Player *m_player;
	TemplateVector<GraphNode*> path;
	// reference the the room
	Room * m_room;
	// internal function used to set up stats based on enemy type
	void setup();
	bool moveCheck();
	void wander();
	void seek(V2DF target);
	bool checkForCollisions();
	GraphNode * getMyPos();
	void findPath();

public:
	Enemy();
	~Enemy();
	void initialize(ENEMY_TYPE a_type, V2DF a_pos, Player* a_player, Room* a_room);
	void initialize(char a_type, V2DF a_pos, Player* a_player, Room* a_room);
	void update(float dT);
	void takeDmg(float dmg);
	float checkHealth();

};