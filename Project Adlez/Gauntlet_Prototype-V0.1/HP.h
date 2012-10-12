#pragma once
#include "Entity.h"
#include "Player.h"

#define MAX_HEARTS 5
class HP : public Entity
{
protected:
	float m_health;
	int m_index[MAX_HEARTS+1];
public:
	HP(void);
	~HP(void);
	void initialize(void);
	void update(float hp);
	void render();
	void setIndex();
};

