#pragma once
#include "Entity.h"
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
	float wanderAngle;
	Player *m_player;
	// reference the the room
	Room * m_room;
	// internal function used to set up stats based on enemy type
	void setup();
	bool moveCheck();
	void wander();
	void seek();
public:
	Enemy();
	~Enemy();
	void initialize(ENEMY_TYPE a_type, V2DF a_pos, Player* a_player);
	void initialize(char a_type, V2DF a_pos, Player* a_player);
	void update(float dT);
	void takeDmg(float dmg);
	float checkHealth();
};