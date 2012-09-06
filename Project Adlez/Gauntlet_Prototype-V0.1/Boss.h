#pragma once
#include "Entity.h"

class Boss : public Entity
{
private:
	float maxHealth;
	float curHeatlh;
	float mSpd;
	float dmg;
	float aSpd;
	float atkRange;
	// Cool Down timer for attack
	ClDwn atkTimer;
	// attack animation timer
	ClDwn aniTimer;
public:
};