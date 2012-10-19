#pragma once
#include "Player.h"
#include "Room.h"
#include "Dungeon.h"
#include "Button.h"
#include "UserInterface.h"

enum G_State {MENU, CLASS, GAME, DEAD, VICTORY};

// contains all game components
class GameEngine
{
private:
	Player thePlayer;
	Dungeon Dungeon1;
	Texture guiTest;
	bool running;
	G_State m_state;
	// menu buttons
	Button quit;
	Button start;
	// class buttons
	Button warrior;
	Button mage;
	// death buttons
	Button main;
	// UI
	UserInterface UI;
	Texture bg;
	Texture adlez;
	Texture gamebg;

public:
	GameEngine();
	~GameEngine();
	void initialize();
	void update(float dT);
	void render();
	void drawText();
	void release();
	bool isRunning();
	void setRunning(bool a_run);
	void setState(G_State state);
};