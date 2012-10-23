#include "UserInterface.h"


UserInterface::UserInterface(void) 
{
}

void UserInterface::initialize() 
{
	hp.initialize();
	gem.initialize();
	items.initialize(V2DF(UI_LEFT, MAINHAND_TOP));
}

void UserInterface::update(STATUS status) 
{
	hp.update(status.health);
	items.update(status.slot1, status.slot2);
	gem.addGems(status.gCount);
}

void UserInterface::render() 
{
	hp.render();
	gem.render();
	items.render();
}

void UserInterface::drawText()
{
	gem.drawText();
}