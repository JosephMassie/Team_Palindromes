#pragma once
#include "Entity.h"

class Treasure : public Entity
{
private:
	static const int MAX_VALUE = 50;
	static const int MIN_VALUE = 25;
	int value;
public:
	void initialize(V2DF a_pos);
	void update(float dT);
	int getValue();
};