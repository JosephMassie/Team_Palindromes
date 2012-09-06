#include "Treasure.h"

void Treasure::initialize(V2DF a_pos)
{
	// set position
	m_pos = a_pos;
	// load texture
	m_tex.initialize(L"treasure.png");
	// set up collision rect
	m_boundRect.top = -8;
	m_boundRect.bottom = 8;
	m_boundRect.left = -8;
	m_boundRect.right = 8;
	// generate a random amount of treasure
	value = randomInt(MIN_VALUE,MAX_VALUE);
	// determine scale
	m_scale = (float)value/(float)MAX_VALUE;
	m_angle = 0;
}

void Treasure::update(float dT)
{
}

int Treasure::getValue()
{
	return value;
}