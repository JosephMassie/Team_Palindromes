#include "Player.h"
#include "Room.h"
#include "GameEngine.h"

// retrieve an instance of input
// default player to warrior
Player::Player()
{
	// set stats to 0
	maxHealth = 100;
	curHealth = 100;
	mSpd = 6;
	dmg = 15;
	aSpd = .5;
	// make sure angle and scale are set to default values
	m_angle = 0.0f;
	m_scale = 1.0f;
}

// Default constructor make sure to call safe release
Player::~Player()
{
	release();
}

// set player's stats based on given class and load the proper texture
void Player::initialize(FRect a_rect)
{
	m_tex.initialize(L"warrior.png");
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
	m_right.left = m_boundRect.right - 5;
	m_right.bottom = m_boundRect.bottom - 3;
	// left
	m_left.top = m_boundRect.top + 3;
	m_left.right = m_boundRect.left + 5;
	m_left.left = m_boundRect.left;
	m_left.bottom = m_boundRect.bottom - 3;
	// top
	m_top.top = m_boundRect.top;
	m_top.right = m_boundRect.right - 3;
	m_top.left = m_boundRect.left + 3;
	m_top.bottom = m_boundRect.top + 5;
	// bottom
	m_bottom.top = m_boundRect.bottom - 5;
	m_bottom.right = m_boundRect.right - 3;
	m_bottom.left = m_boundRect.left + 3;
	m_bottom.bottom = m_boundRect.bottom;
	// set the base entity components of the player
	m_pos = V2DF(GRID_SIZE+HALF_GRID,GRID_SIZE+HALF_GRID);
	m_angle = 0.0f;
	m_scale = 1.0f;
	
	// set up can shoot cool down
	canShoot.active = false;
	canShoot.duration = aSpd;
	canShoot.timePassed = 0.0f;
}

// internal function to deside if the next space is enterable
// Return true if no collisions accured and false otherwise
bool Player::collisionCheck()
{
	// temp value used to store position of collided objects
	V2DF pen(0,0); // initialized to 1,1 for the collision functions
	bool collided = false; // used to control collision so multiple sides aren't checked preventing errors
	FRect checkRect = getRelativeBoundRect();
	// first check if the player is colliding in general with terrain
	if(m_curRoom->collImpassable(checkRect, m_pos, &pen,RIGHT))
	{
		// if the collision is on the right side
		// make sure to reset penetration
		pen = V2DF(0,0);
		checkRect = getSubRect(RIGHT);
		if(m_curRoom->collImpassable(checkRect, m_pos, &pen, RIGHT))
		{
			// augment x position by the x componet of the penetration vector
			m_pos.x -= pen.x;
		}
		// if the collision is on the left side
		// make sure to reset penetration
		pen = V2DF(0,0);
		checkRect = getSubRect(LEFT);
		if(m_curRoom->collImpassable(checkRect, m_pos, &pen, LEFT))
		{
			// augment x position by the x componet of the penetration vector
			m_pos.x -= pen.x;
		}
		// if the collision is on the top side
		// make sure to reset penetration
		pen = V2DF(0,0);
		checkRect = getSubRect(TOP);
		if(m_curRoom->collImpassable(checkRect, m_pos, &pen, TOP))
		{
			// augment x position by the x componet of the penetration vector
			m_pos.y -= pen.y;
		}
		// if the collision is on the bottom side
		// make sure to reset penetration
		pen = V2DF(0,0);
		checkRect = getSubRect(BOT);
		if(m_curRoom->collImpassable(checkRect, m_pos, &pen, BOT))
		{
			// augment x position by the x componet of the penetration vector
			m_pos.y -= pen.y;
		}
	}
	
	// now return whether a collision has occured
	return collided;
}

void Player::render()
{
	// render player
	m_tex.draw(m_pos, m_angle, m_scale);
}

// used to keep inheritance happy
void Player::update(float dT)
{
}

// Gather input and update the player accordingly
void Player::Update(float dT, GameEngine* engine)
{
	// if health is bellow 0 end game
	if(curHealth <= 0)
	{
		engine->setState(DEAD);
	}
	// update can shoot cool down
	CoolDownCtrl(dT, canShoot);

	// always orientate facing to the mouse pointer
	V2DF mouse = pInput->getMousePos();
	V2DF difference = mouse.difference(m_pos);
	// add a little fudge factor for error
	m_angle = atan2f(difference.y,difference.x)*(180.0f/V2D_PI) + 90.0f;

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
	collisionCheck();
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
	pSound->playSound(HIT);
	curHealth-=a_dmg;
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
}

bool Player::dmged()
{
	return (curHealth < maxHealth);
}