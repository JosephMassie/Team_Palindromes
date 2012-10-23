#pragma once
#include "Entity.h"

class Rock : public Entity
{
private:
	float value;
public:
	void update(float dT);
	void addForce(V2DF force);
	void setForce(V2DF force);
};