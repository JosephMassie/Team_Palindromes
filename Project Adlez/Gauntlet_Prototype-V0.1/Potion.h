#pragma once
#include "Entity.h"

class Potion : public Entity
{
private:
public:
	Potion();
	void initialize(V2DF a_pos);
	void update(float dT);
};