#pragma once
#include "Entity.h"
#include "Input.h"
#include "Sound.h"
#include "Items.h"
extern class Room;
extern class GameEngine;

enum QUADRANT {N, NE, E, SE, S, SW, W, NW};

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

	Items pItems;
	ClDwn b1Cldwn;
	ClDwn b2Cldwn;

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

	void drawText();

	QUADRANT getQuadrant(); //quadrant where the player is facing

	//bools if button 1 or 2 is pressed
	bool button1;
	bool button2;

	//sets the item on specified slot
	void setItem1();
	void setItem2();

	//equips item on specified slot
	ITEM equipItem1();
	ITEM equipItem2();
	ITEM item1;
	ITEM item2;

	int slot1;
	int slot2;


	/////////////DEBUGGING/////////////////
	char hit;
};
