#pragma once
#include "Entity.h"
extern class Player;

enum ITEM {SWORD,SHIELD,BOOMERANG,BOMB};

#define SWCD 0.5 //sword cooldown
#define SHCD 1 //shield cooldown
#define BMCD 2 //boomerang cooldown
#define BMBCD 3 //Bomb cooldown

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
	Player* player;

};