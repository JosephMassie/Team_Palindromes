#pragma once
#include "Entity.h"

class Gem : public Entity
{
protected:
	int m_count;
	DX2DEngine* engine;
public:
	Gem(void);
	~Gem(void);
	void initialize();
	void update(int count);
	void render();
	void drawText();
};

