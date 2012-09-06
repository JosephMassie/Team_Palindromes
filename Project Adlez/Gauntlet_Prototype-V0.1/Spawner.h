#pragma once
#include "Enemy.h"
extern class Player;
extern class Room;

class Spawner : public Entity
{
private:
	// references to the room and player
	Room* m_room;
	Player* thePlayer;
	// enemy control variables
	int max_enemies;
	TemplateVector<Enemy*> m_enemies;
	ClDwn canSpawn;
	ENEMY_TYPE spawnType;
	V2DF m_roomPOS;
	// spawner stats
	float maxHealth;
	float curHealth;
	bool alive;
public:
	void initialize(Room* a_room, Player* player, int floorNum, V2DF a_pos, V2DF rPOS);
	void update(float dT);
	void render();
	void takeDmg(float a_dmg);
	float checkHealth();
	bool checkIfLiving();
	bool collEnemies(FRect a_rect, float a_dmg, V2DF pos, V2DF* pen, SIDE side);
	bool collEnemies(FRect a_rect, float a_dmg);
};