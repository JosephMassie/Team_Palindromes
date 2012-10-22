#include "Player.h"
#include "Room.h"
#include "GameEngine.h"

// retrieve an instance of input
// default player to warrior
Player::Player()
{
	// set stats to 0
	maxHealth = 10;
	curHealth = 10;
	mSpd = 6;
	dmg = 15;
	aSpd = .5;
	// make sure angle and scale are set to default values
	m_angle = 0.0f;
	m_scale = 1.0f;

	//Debugging
	item1 = SWORD;
	item2 = SHIELD;

	slot1 = 0;
	slot2 = 1;
	hit = 'N';
}

// Default constructor make sure to call safe release
Player::~Player()
{
	release();
}

// set player's stats based on given class and load the proper texture
void Player::initialize(FRect a_rect)
{
	m_tex.initialize(L"images/adlez_walking.png", 8, 2, 4, 32);
	playerAnim.initialize(&m_tex, 40.0f);
	// get references to input, sound, and graphics engines
	pInput = Input::getInstance();
	pSound = Sound::getInstance();
	gEngine = DX2DEngine::getInstance();
	// set bound rect
	m_boundRect = a_rect;
	// set up sub bound rects
	// right
	m_right.top = m_boundRect.top + 3;
	m_right.right = m_boundRect.right;
	m_right.left = m_boundRect.right - 3;
	m_right.bottom = m_boundRect.bottom - 3;
	// left
	m_left.top = m_boundRect.top + 3;
	m_left.right = m_boundRect.left + 3;
	m_left.left = m_boundRect.left;
	m_left.bottom = m_boundRect.bottom - 3;
	// top
	m_top.top = m_boundRect.top;
	m_top.right = m_boundRect.right - 3;
	m_top.left = m_boundRect.left + 3;
	m_top.bottom = m_boundRect.top + 3;
	// bottom
	m_bottom.top = m_boundRect.bottom - 3;
	m_bottom.right = m_boundRect.right - 3;
	m_bottom.left = m_boundRect.left + 3;
	m_bottom.bottom = m_boundRect.bottom;
	// set the base entity components of the player
	m_pos = V2DF(GRID_SIZE+HALF_GRID,GRID_SIZE+HALF_GRID);
	m_angle = 0.0f;
	m_scale = 0.8f;
	
	// set up can shoot cool down
	canShoot.active = false;
	canShoot.duration = aSpd;
	canShoot.timePassed = 0.0f;

	b1Cldwn.active = false;
	b1Cldwn.timePassed = 0.0f;
	b2Cldwn.timePassed = 0.0f;
	b2Cldwn.active = false;

	pItems = Items(this);
}

// internal function to deside if the next space is enterable
// Return true if no collisions accured and false otherwise
bool Player::checkForCollisions()
{
	bool collided = false;
	// check against all walls
	collided = m_curRoom->coll_walls(this);
	return collided;
}

void Player::render()
{
	// render player
//	m_tex.draw(m_pos, m_angle, m_scale);
	playerAnim.render(m_pos, m_angle, m_scale);

	//render items
	pItems.render();
}

// used to keep inheritance happy
void Player::update(float dT)
{
	
}

// Gather input and update the player accordingly
void Player::Update(float dT, GameEngine* engine)
{
	b1Cldwn.duration = pItems.I1Cldwn;
	b2Cldwn.duration = pItems.I2Cldwn;

	// if health is bellow 0 end game
	if(curHealth <= 0)
	{
		engine->setState(DEAD);
	}
	// update can shoot cool down
	CoolDownCtrl(dT, canShoot);
	CoolDownCtrl(dT, b1Cldwn);
	CoolDownCtrl(dT, b2Cldwn);

	// always orientate facing to the mouse pointer
	V2DF mouse = pInput->getMousePos();
	V2DF difference = mouse.difference(m_pos);
	// add a little fudge factor for error
	m_angle = atan2f(difference.y,difference.x)*(180.0f/V2D_PI) + 90.0f;
	// make sure m_angle is positive
	if(m_angle < 0.0f)
		m_angle += 360.0f;

	// get key input
	getInput();

	// make sure velcoity does not exceed maz
	if(velocity.length() > mSpd * GRID_SIZE)
		velocity = velocity.normal().product(mSpd * GRID_SIZE);

	// update position based on velocity
	V2DF temp = velocity.product( dT );
	m_pos.add(temp);

	// degrade velocity by 20%
	velocity.multiply(.8);

	// now check if anything has collided
	checkForCollisions();
	resolveCollisions(); // if a collision occured resolve it

	// update player animation
	playerAnim.update(dT);
	if(velocity.isLessThan(.5))
	{
		playerAnim.stop(true);
	}
	else
	{
		playerAnim.start(false);
	}
	//update Items
	pItems.update(dT);
}

void Player::setCurrentRoom(Room* a_room)
{
	m_curRoom = a_room;
}

Room* Player::getCurrentRoom()
{
	return m_curRoom;
}

void Player::takeDmg(float a_dmg)
{
//	pSound->playSound(HIT);
//	curHealth-=a_dmg;
}

void Player::getInput()
{
	// get key input
	// UP
	if (pInput->keyDown(DIK_W))
	{
		velocity.add( V2DF(0,-GRID_SIZE));
	}
	// LEFT
	if (pInput->keyDown(DIK_A))
	{
		velocity.add( V2DF(-GRID_SIZE,0));
	}
	// DOWN
	if (pInput->keyDown(DIK_S))
	{
		velocity.add( V2DF(0,GRID_SIZE));
	}
	// RIGHT
	if (pInput->keyDown(DIK_D))
	{
		velocity.add( V2DF(GRID_SIZE,0));
	}
	
	//if the buttons are pressed use the appropriate item slot
	if(pInput->mouseButtonDown(0) && !b2Cldwn.active && !b1Cldwn.active ) {
		pItems.useItem1(item1);
		b1Cldwn.active = true;
	}
	
	if(pInput->mouseButtonDown(1) && !b2Cldwn.active && !b1Cldwn.active) {
		pItems.useItem2(item2);
		b2Cldwn.active = true;
	}
/////////////////////////////////////////////////
/////    Weapon Switching
///////////////////////////////////////////////
	///// Q to cycle through slot 1
	if(pInput->keyPressed(DIK_Q)&& !b2Cldwn.active && !b1Cldwn.active) {
		slot1++;
		if(slot1 == slot2){
			slot1++;
		}
		if(slot1 >= 4) {
			slot1 = 0;
			if(slot1 == slot2){
				slot1++;
			}
		}
		if(slot1 == 0) {
			item1 = SWORD;
		}
		else if(slot1 == 1) {
			item1 = SHIELD;
		}
		else if(slot1 == 2) {
			item1 = BOOMERANG;
		}
		else if(slot1 == 3) {
			item1 = BOMB;
		}
	}
	// E to cycle throught slot 
	if(pInput->keyPressed(DIK_E)&& !b2Cldwn.active && !b1Cldwn.active) {
		slot2++;
		if(slot2 == slot1){
			slot2++;
		}
		if(slot2 >= 4) {
			slot2 = 0;
			if(slot2 == slot1){
				slot2++;
			}
		}
		if(slot2 == 0) {
			item2 = SWORD;
		}
		else if(slot2 == 1) {
			item2 = SHIELD;
		}
		else if(slot2 == 2) {
			item2 = BOOMERANG;
		}
		else if(slot2 == 3) {
			item2 = BOMB;
		}
	}
	// check the the player's current space is an unlocked door
	// used to get next room
	V2DF posNew(0,0);
	Room* nextRoom = m_curRoom->collOpenDoors(getRelativeBoundRect(), posNew);	
	// if so move the player to the next room
	// if the next room exists go to it
	if(nextRoom)
	{
		m_curRoom = nextRoom;
		// now "enter" the new room
		m_curRoom->enterRoom(this);
		// set new position
		m_pos.x = (posNew.x * GRID_SIZE) + HALF_GRID + BORDER;
		m_pos.y = (posNew.y * GRID_SIZE) + HALF_GRID + BORDER;
	}

	// ................ UI Debug
	// Press 'F' to lower health
	if(pInput->keyPressed(DIK_F) && curHealth!=0)
	{
		curHealth--;
	}
	// Press 'G' to increase health
	if(pInput->keyPressed(DIK_G) && curHealth<maxHealth)
	{
		curHealth++;
	}
}

bool Player::dmged()
{
	return (curHealth < maxHealth);
}

QUADRANT Player::getQuadrant() {
	QUADRANT temp;
	float angle = getAngle();
	if(angle >= 337.6 && angle < 360 || angle > 0 && angle <= 22.5) {
		temp = N;
	}
	else if(angle <= 337.5 && angle >= 292.6) {
		temp = NW;
	}
	else if(angle <= 292.5 && angle >= 247.6) {
		temp = W;
	}
	else if(angle <= 247.5 && angle >= 202.6) {
		temp = SW;
	}
	else if(angle <= 202.5 && angle >= 157.6) {
		temp = S;
	}
	else if(angle <= 157.5 && angle >= 112.6) {
		temp = SE;
	}
	else if(angle <= 112.5 && angle >= 67.5) {
		temp = E;
	}
	else {
		temp = NE;
	}
	return temp;
}

void Player::setItem1() {
	item1 = equipItem1();
}

void Player::setItem2() {
	item2 = equipItem2();
}

ITEM Player::equipItem1() {
	//equip script
	ITEM temp;
	temp = SWORD; //debugging
	return temp;
}

ITEM Player::equipItem2() {
	//equip script
	ITEM temp;
	temp = SHIELD; //debugging
	return temp;
}

//////////////////////////////////////
//////Debugging
/////////////////////////////////////
void Player::drawText() {
	char buff[200] = {0};
	char cd1, cd2;
	if(b1Cldwn.active) {
		cd1 = 'Y';
	}
	else {
		cd1 = 'N';
	}
	if(b2Cldwn.active) {
		cd2 = 'Y';
	}
	else {
		cd2 = 'N';
	}
	sprintf(buff,"angle: %d \nquad: %d\nItem1 active: %d\nItem2 active: %d\nItem1 type: %d \nItem2 type: %d\nHit : %c",(int)m_angle,getQuadrant(),(int)b1Cldwn.duration,(int)b2Cldwn.duration,(int)slot1,(int)slot2, hit);
	RECT temp;
	temp.bottom = 600;
	temp.top = 0;
	temp.left = 0;
	temp.right = 800;
	gEngine->writeText(buff, temp);
}

//////////////////////////
// UI specific
//////////////////////////
STATUS Player::getStatus()
{
	status.health = curHealth;
	status.slot1 = slot1;
	status.slot2 = slot2;
	return status;
}