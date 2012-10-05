#include "Items.h"
#include "Player.h"
#include "Room.h"

Items::Items() {
	I1Cldwn = 0;
	I2Cldwn = 0;
}
Items::Items(Player *a_player) {
	I1Cldwn = 0;
	I2Cldwn = 0;
	player = a_player;
}

//Use item depending what item is on slot 1
void Items::useItem1(ITEM item) {
	switch(item) {
	case SWORD: 
		useSword();
		I1Cldwn = SWCD; 
		break;
	case SHIELD:
		useShield();
		I1Cldwn = SHCD; 
		break;
	case BOOMERANG:
		useBoomerang();
		I1Cldwn = BMCD; 
		break;
	case BOMB:
		useBomb();
		I1Cldwn= BMBCD;
		break;
	}
}

//Use item depending what item is on slot 2
void Items::useItem2(ITEM item) {
	switch(item) {
	case SWORD: 
		useSword();
		I2Cldwn = SWCD; 
		break;
	case SHIELD:
		useShield();
		I2Cldwn = SHCD;
		break;
	case BOOMERANG:
		useBoomerang();
		I2Cldwn = BMCD; 
		break;
	case BOMB:
		useBomb();
		I2Cldwn= BMBCD;
		break;
	}
}

void Items::useSword() {
	//sword script
	// Check if any entity other than the player is colliding with the line/sword
	 //draw the line where the player is facing
	 //check if any entity is colliding with it
	Room* tempRoom;
	tempRoom = player->getCurrentRoom();
	float tempAngle =( player->getAngle()) * (V2D_PI / 180.0f);
	V2DF swordPos(cos(tempAngle), sin(tempAngle));
	V2DF tempColPos;
	swordPos.normalize();
	swordPos.multiply(100);
	swordPos.add(player->getPosition());
	for(int i = 0; i < tempRoom->EnemyCount(); i++) {
		if(swordPos.lineCrossesCircle(swordPos, player->getPosition(), tempRoom->getEnemy(i)->getPosition(), 30,tempColPos)) {
			player->hit = 'Y';
			break;
		}
		else {
			player->hit = 'N';
		}
	}


}
void Items::useShield() {
	//shield script
	/*switch(player->getQuadrant()) {
	case N:
		break;
	case NE:
		break;
	case E:
		break;
	case SE:
		break;
	case S:
		break;
	case SW:
		break;
	case W:
		break;
	case NW:
		break;
	}
	*/
	//check what quadrant the player is facing
	//if damage will occur from that direction make it do no damage
	//minimum active time is 0.5 seconds and max is till player releases button
}
void Items::useBoomerang() {
	//boomerang script
	//travel through a certain path mostly oval
	//if an entity hits it that entity takes damage
	//cooldown is refreshed when the boomerang is back at the player's position
}

void Items::useBomb() {
	//bomb script
	//leaves a bomb at the player's position
	//bomb activates and explodes after a few seconds
}
