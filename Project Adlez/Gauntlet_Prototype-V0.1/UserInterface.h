#pragma once
#include "Button.h"
#include "HP.h"
#include "Gem.h"
#include "Inventory.h"
#include "Input.h"

// UI position adjustments
#define MAINHAND_TOP 140
#define OFFHAND_TOP 180

class UserInterface {
protected:
	HP hp;
	Gem gem;
	Inventory items;
public:
	UserInterface(void);
	void initialize();
	void update(STATUS status);
	void render();
	void drawText();
};

