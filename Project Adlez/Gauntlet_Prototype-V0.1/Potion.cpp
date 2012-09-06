#include "Potion.h"

Potion::Potion()
{
}

void Potion::initialize(V2DF a_pos)
{
	m_pos = a_pos;
	m_scale = 1.0f;
	m_angle = 0.0f;
	m_tex.initialize(L"potion.png");
	// set up collision rect
	m_boundRect.top = -8;
	m_boundRect.bottom = 8;
	m_boundRect.left = -8;
	m_boundRect.right = 8;
}

void Potion::update(float dT)
{
}