#pragma once
#include "Entity.h"
extern class Player;

enum ITEM {SWORD,SHIELD,BOOMERANG,BOMB};

#define SWCD 0.5 //sword cooldown
#define SHCD 1 //shield cooldown
#define BMCD 2 //boomerang cooldown
#define BMBCD 3.2 //Bomb cooldown

class Items {
public:
	Items();
	Items(Player *a_player);
	~Items(){ };

	float I1Cldwn;
	float I2Cldwn;
	
	void useItem1(ITEM item);
	void useItem2(ITEM item);
	
	void useSword();
	void useShield();
	void useBoomerang();
	void useBomb();
	void updateBomb(float dT);
	void updateBoom(float dT);
	void update(float dT);
	FRect createRect(float width, float height);
	Player* player;
	
	Entity boom;
	FRect boomRect;
	ClDwn boomCldwn;
	
	Entity bomb;
	ClDwn bombCldwn;
	FRect bombRect;
	
	int bombState;
	void render();
	//bool checkCollision(Entity *item,Entity *other);
};