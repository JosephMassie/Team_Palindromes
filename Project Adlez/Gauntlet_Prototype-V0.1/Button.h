#pragma once
#include "HelpfulData.h"
#include "Input.h"
#include "DX2DEngine.h"
#include "Texture.h"
#include "Sound.h"

class Button
{
private:
	Sound* theSound;
	FRect bRect;
	V2DF m_pos;
	Input * input;
	DX2DEngine * engine;
	Texture m_idleTex;
	Texture m_hoverTex;
	char * text;
	int length;
	bool hover;
	bool selected;
	bool click;
public:
	Button();
	void initialize(FRect a_rect, V2DF m_pos);
	void update(float dT);
	void setText(char * text, int count);
	void render();
	void drawText();
	bool clicked();
};