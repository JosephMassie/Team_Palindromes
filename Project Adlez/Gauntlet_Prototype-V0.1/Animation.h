#pragma once
#include "Texture.h"
#include "HelpfulData.h"

class Animation
{
private:
	ClDwn m_frameSwitch;
	Texture * m_texture;
	int currentFrame;
	bool begining;
	bool play;
public:
	Animation();
	~Animation();
	void initialize(Texture * a_texture, float fps);
	void render(V2DF pos, float angle, float scale);
	void update(float dT);
	void start(bool reset);
	void stop(bool reset);
};