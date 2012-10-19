#pragma once
#include <vector>
#include "Entity.h"

#define SLOTS 2
#define NUM_ITEMS 4

struct INV
{
	V2DF pos;
	float angle;
	float scale;
};

class Inventory
{
private:	
	INV cur[SLOTS];
	INV next[SLOTS];
	INV next2[SLOTS];
	Texture sword;
	Texture shield;
	Texture bomb;
	Texture boomerang;
	int slotcur[SLOTS], slotnext[SLOTS], slotnext2[SLOTS];
	int slot1, slot2;
	int count;
public:
	Inventory(void);
	~Inventory(void);

	void initialize(V2DF pos);

	void update(int a_slot1, int a_slot2);
	void render();

	void release();
};

