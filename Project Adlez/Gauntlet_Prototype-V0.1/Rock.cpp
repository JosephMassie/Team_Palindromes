#include "Rock.h"
	
void Rock::update(float dT)
{
}

void Rock::addForce(V2DF force)
{
	velocity += force;
}

void Rock::setForce(V2DF force)
{
	velocity = force;
}