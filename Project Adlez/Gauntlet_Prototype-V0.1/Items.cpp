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
	bombState = 0;
	player = a_player;
	bombRect = createRect(GRID_SIZE * 2,GRID_SIZE  *2);
	//boomRect = createRect(0,GRID_SIZE,0,GRID_SIZE);
	bomb.initialize(V2DF(0,0),L"images/bomb.png",bombRect,0,1);
	boom.initialize(V2DF(0,0),L"images/boomerang.png",boomRect,0,1);

	bombCldwn.active = false;
	bombCldwn.duration = BMBCD;
	bombCldwn.timePassed = 0;

	boomCldwn.active = false;
	boomCldwn.duration = BMCD;
	boomCldwn.timePassed = 0;

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
	float tempAngle =( player->getAngle() - 90) * (V2D_PI / 180.0f);
	V2DF swordPos(cos(tempAngle), sin(tempAngle));
	V2DF tempColPos;
	swordPos.normalize();
	swordPos.multiply(35);
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
	boom.setPosition(player->getPosition());
	boomCldwn.active = true;
}

void Items::useBomb() {
	//leaves a bomb at the player's position
	bomb.setPosition(player->getPosition());
	bombCldwn.active = true;
	bombState = 1;
}

void Items::update(float dT) {
	updateBomb(dT);
	updateBoom(dT);
}
void Items::updateBoom(float dT) {
	Room* tempRoom;
	tempRoom = player->getCurrentRoom();
	if(boomCldwn.active) {
		CoolDownCtrl(dT,boomCldwn);
		if(boomCldwn.active == false) {
			for(int i = 0; i < tempRoom->EnemyCount(); i++) {
				//if(/*checkCollision(&boom,tempRoom->getEnemy(i)*/) ) {
				//	//do damage to enemy here
				//}
			}
		}

	}
	
}
void Items::updateBomb(float dT) {
	//if bomb is activated
	  //set timer
	  //blow up after timer is done
	//check for collisions when it blows up
	Room* tempRoom;
	tempRoom = player->getCurrentRoom();
	if(bombState == 1) {
		CoolDownCtrl(dT, bombCldwn);
		if(bombCldwn.active == false) {
			for(int i = 0; i < tempRoom->EnemyCount(); i++) {
				if(tempRoom->getEnemy(i)->collisionCheck(&bomb, false) ){
					//do damage to enemy here
					player->hit = 'Y';
				}
			}
			bombState = 0;
		}

	}
}

//bool Items::checkCollision(Entity *item,Entity* other) {
//	FRect rectA = item->getRelativeBoundRect();
//	FRect rectB = other->getRelativeBoundRect();
//	bool collide;
//	if( colliding(rectA, rectB) )	{
//		return (collide = true);
//	}
//	return (collide = false);
//}

void Items::render() {
	if(bombCldwn.active) {
		bomb.render();
	}
	if(boomCldwn.active) {
		boom.render();
	}
}

FRect Items::createRect(float width, float height) {
	FRect temp;
	temp.top = -(height /2) ;
	temp.bottom = (height /2);
	temp.left = -(width / 2);
	temp.right = width / 2;
	return temp;
}