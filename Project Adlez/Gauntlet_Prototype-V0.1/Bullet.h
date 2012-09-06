#pragma once
#include "Entity.h"
extern class Room;

class Bullet : public Entity
{
private:
	V2DF trajectory;
	Room* m_room;
	static const int speed = 10;
	float m_dmg;
public:
	Bullet();
	~Bullet();
	void initialize(V2DF a_trajectory, V2DF a_pos, Room* a_room, float a_dmg);
	bool Update(float dT);
	void update(float dT);
};