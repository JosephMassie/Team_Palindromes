#pragma once
#include "Entity.h"
#include "Input.h"
#include "Sound.h"
extern class Room;
extern class GameEngine;

class Player : public Entity
{
private:
	Input* pInput;
	Sound* pSound;
	Room* m_curRoom;
	DX2DEngine* gEngine;
	//--------------
	// Player Stats
	float maxHealth;
	float curHealth;
	float mSpd;
	float dmg;
	float aSpd;
	//---------------
	bool checkForCollisions();
	ClDwn canShoot;
	void getInput();
public:
	Player();
	~Player();
	void update(float dT);
	void Update(float dT, GameEngine* engine);
	void render();
	void initialize(FRect a_rect);
	void setCurrentRoom(Room* a_room);
	Room* getCurrentRoom();
	void takeDmg(float a_dmg);
	bool dmged();
};