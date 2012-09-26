#include "Enemy.h"
#include "Room.h"
#include "Player.h"

// set pointers to 0
Enemy::Enemy()
{
	m_player = 0;
	m_room = 0;
}

// make sure safer release is called
Enemy::~Enemy()
{
	release();
}

// set stats based on type and set player reference
void Enemy::initialize(ENEMY_TYPE a_type, Player* a_player, V2DF a_pos, Room * a_room)
{
	// attack timer
	atkTimer.active = false;
	atkTimer.duration = 0.5f;
	atkTimer.timePassed = 0.0f;
	// atk animation timer
	aniTimer.active = false;
	aniTimer.duration = 0.2f;
	aniTimer.timePassed = 0.0f;
	// ultamate atk timer BOSS ONLY
	ultTimer.active = false;
	ultTimer.duration = 2.0f;
	ultTimer.timePassed = 0.0f;
	m_pos = a_pos;
	// set player reference
	m_player = a_player;
	// set the room reference
	m_room = a_room;
	// set collisioin rect
	FRect temp; temp.bottom = 8; temp.left = -8; temp.right = 8; temp.top = -8;
	m_boundRect = temp;
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
	m_top.right = m_boundRect.right;
	m_top.left = m_boundRect.left;
	m_top.top = m_boundRect.top + 3;
	// bottom
	m_bottom.top = m_boundRect.bottom - 3;
	m_bottom.right = m_boundRect.right;
	m_bottom.left = m_boundRect.left;
	m_bottom.bottom = m_boundRect.bottom;
	// set type and stats
	m_type = a_type;
	setup();
	// set scale and rotation to defaults
	m_angle = 0.0f;
	m_scale = 1.0f;
	// set control variables
	velocity = V2DF(0.0f,0.0f);
	steeringForce = V2DF(0,0);
	wanderAngle = randomInt(1,360);
}

void Enemy::setup()
{
	switch(m_type)
	{
	case GHOST:
		// set stats to that of a ghost
		m_tex.initialize(L"ghost.png");
		maxHealth = 30;
		curHeatlh = maxHealth;
		mSpd = 2;
		dmg = 4;
		aSpd = 1;
		atkRange = 24;
		siteRange = GRID_SIZE * 8;
		break;
	case BOSS:
		// set stats to boss
		m_tex.initialize(L"boss.png");
		maxHealth = 200;
		curHeatlh = maxHealth;
		mSpd = .5;
		dmg = 10;
		atkTimer.duration = 1.0f;
		aSpd = 1;
		atkRange = 80;
		siteRange = 0; // not use for boss
		// boss needs a bigger hit box
		m_boundRect.top = -32;
		m_boundRect.left = -32;
		m_boundRect.bottom = 32;
		m_boundRect.right = 32;
		break;
	}
}

void Enemy::takeDmg(float a_dmg)
{
	curHeatlh-=a_dmg;
}

float Enemy::checkHealth()
{
	return curHeatlh;
}

void Enemy::update(float dT)
{
	CoolDownCtrl(dT,atkTimer);
	CoolDownCtrl(dT,aniTimer);
	CoolDownCtrl(dT,ultTimer);
	// if not boss work normally
	if(m_type != BOSS)
	{
		// check if player is in attack range
		if (m_player->getPosition().isWithin(atkRange, m_pos) && !atkTimer.active)
		{
			m_player->takeDmg(dmg);
			atkTimer.active = true;
			aniTimer.active = true;
		}
		else if (!m_player->getPosition().isWithin(atkRange, m_pos))
		{
			bool wandering = false;
			// see if the player is in sight range
			if (m_player->getPosition().isWithin(siteRange,m_pos))
				seek();
			else
			{
				wander();
				wandering = true;
			}
			// try and avoid nearby walls
			V2DF avoid(0,0);
			if(wandering)
				velocity.add(steeringForce);
			
		}
	}
	else
	{
		// check if player is in attack range
		if (m_player->getPosition().isWithin(atkRange, m_pos) && !atkTimer.active)
		{
			m_player->takeDmg(dmg);
			atkTimer.active = true;
			aniTimer.active = true;
		}
		// use ultimate attack if possible
		else if(m_player->getPosition().isWithin(atkRange, m_pos) && !ultTimer.active && (randomInt(0,100) >= 60))
		{
			m_player->takeDmg(dmg*2);
			ultTimer.active = true;
			aniTimer.active = true;
		}
		// always move towards the player
		seek();
		V2DF avoid(0,0);
		
	}

	// make sure velocity doesn't exceed max
	if (velocity.length() > mSpd * GRID_SIZE)
		velocity = velocity.normal().product(mSpd * GRID_SIZE);
	// update position
	m_pos.add( velocity.product(dT) );
	// degrade velocity by 20%
	velocity.multiply(.8);
	// change facing based on velocity
	m_angle = atan2f(velocity.y,velocity.x)*(180.0f/V2D_PI) + 90.0f;

	// control attack animation
	if(aniTimer.active)
		m_tex.setTint(125,20,20);
	else
		m_tex.setTint(255,255,255);
}

void Enemy::wander()
{
	// set wander angle to current velocity if it is not zero
//	if(!velocity.isZero())
//		wanderAngle = atan2f(velocity.y,velocity.x)*(180.0f/V2D_PI) + 90.0f;
	// temporaly convert angle to radians
	float angleDeg = (float)randomInt(-10,10)/10.0f;
	wanderAngle += angleDeg;
	float angleRad = (V2D_PI / 180.0f) * wanderAngle;
	// temporaly generate a unity vector at the generated angle and a random magnitude
	// set steering force equal to that vector
	steeringForce = V2DF( cos(angleRad), sin(angleRad) ).product( 10 );
}

void Enemy::seek()
{
	V2DF target = m_player->getPosition();

	// go towards target
	// determine current velocity
	//V2DF vEnd = m_pos.sum(velocity);
	// determine current steering force
	V2DF steeringForce = target.difference(m_pos);
	velocity.add(steeringForce);
}