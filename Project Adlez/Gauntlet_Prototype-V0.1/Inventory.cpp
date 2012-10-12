#include "Inventory.h"

void Inventory::initialize(V2DF pos)
{
	// load images
	sword.initialize(L"images/sword.png");
	shield.initialize(L"images/shield.png");
	bomb.initialize(L"images/bomb.png");
	boomerang.initialize(L"images/boomerang.png");

	count = 0;

	//set inventory data
	cur[0].pos = V2DF(pos.x+30, pos.y);
	cur[0].angle = 0.0f;
	cur[0].scale = 1.0f;
	cur[1].pos = V2DF(pos.x+30, pos.y+40);
	cur[1].angle = 0.0f;
	cur[1].scale = 1.0f;
	next[0].pos = V2DF(pos.x+60, pos.y);
	next[0].angle = 0.0f;
	next[0].scale = 0.5f;
	next[1].pos = V2DF(pos.x+60, pos.y+40);
	next[1].angle = 0.0f;
	next[1].scale = 0.5f;
}

Inventory::Inventory(void)
{
}

Inventory::~Inventory(void)
{
}

// update current, previous, and next item pointers
void Inventory::update(int a_slot1, int a_slot2)
{
	slotcur[0] = a_slot1;
	slotcur[1] = a_slot2;
	slotnext[0] = a_slot1+1;
	slotnext[1] = a_slot2+1;
	for(int i=0; i<NUM_ITEMS; ++i)
	{
		if(slotnext[i] > NUM_ITEMS)
		{
			slotnext[i] = 0;
		}
	}
}

void Inventory::render()
{
	for(int i=0; i<SLOTS; ++i)
	{
		switch(slotcur[i])
		{
		case 0:
			sword.draw(cur[i].pos, cur[i].angle, cur[i].scale);
			break;
		case 1:
			shield.draw(cur[i].pos, cur[i].angle, cur[i].scale);
			break;
		case 2:
			boomerang.draw(cur[i].pos, cur[i].angle, cur[i].scale);
			break;
		case 3:
			bomb.draw(cur[i].pos, cur[i].angle, cur[i].scale);
			break;
		}
		switch(slotnext[i])
		{
		case 0:
			sword.draw(next[i].pos, next[i].angle, next[i].scale);
			break;
		case 1:
			shield.draw(next[i].pos, next[i].angle, next[i].scale);
			break;
		case 2:
			boomerang.draw(next[i].pos, next[i].angle, next[i].scale);
			break;
		case 3:
			bomb.draw(next[i].pos, next[i].angle, next[i].scale);
			break;	
		}
	}
}