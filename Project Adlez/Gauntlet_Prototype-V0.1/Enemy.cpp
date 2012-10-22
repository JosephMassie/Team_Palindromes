#include "Enemy.h"
#include "Room.h"
#include "Player.h"
#include "astar.h"

// set pointers to 0
Enemy::Enemy()
{
	m_player = 0;
	m_room = 0;

}

// make sure safer release is called
Enemy::~Enemy()
{
	m_nodeTex.release();
	release();
}

// calls initialize based on character as type rather than the type enum
void Enemy::initialize(char a_type, V2DF a_pos, Player* a_player, Room* a_room)
{
	switch(a_type)
	{
	case 'G':
		initialize(GHOST, a_pos, a_player, a_room);
		break;
	};
}

// set stats based on type and set player reference
void Enemy::initialize(ENEMY_TYPE a_type, V2DF a_pos, Player* a_player, Room* a_room)
{
	m_room = a_room;
	m_player = a_player;
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
	m_top.right = m_boundRect.right - 3;
	m_top.left = m_boundRect.left + 3;
	m_top.bottom = m_boundRect.top + 3;
	// bottom
	m_bottom.top = m_boundRect.bottom - 3;
	m_bottom.right = m_boundRect.right - 3;
	m_bottom.left = m_boundRect.left + 3;
	m_bottom.bottom = m_boundRect.bottom;
	// set type and stats
	m_type = a_type;
	setup();
	// set scale and rotation to defaults
	m_angle = 0.0f;
	m_scale = 0.7f;
	// set control variables
	velocity = V2DF(0.0f,0.0f);
	steeringForce = V2DF(0,0);
	wanderAngle = randomInt(1,360);
	path.clear();

	// debug stuff
	m_nodeTex.initialize(L"images/node.png");
}

void Enemy::setup()
{
	switch(m_type)
	{
	case GHOST:
		// set stats to that of a ghost
		m_tex.initialize(L"images/ghost_anim.png", 8, 2, 4, 32);
		ghostAnim.initialize(&m_tex, 100.0f);
		maxHealth = 30;
		curHeatlh = maxHealth;
		mSpd = 2;
		dmg = 4;
		aSpd = 1;
		atkRange = 50;
		siteRange = GRID_SIZE * 1.1;
		break;
	case BOSS:
		// set stats to boss
		m_tex.initialize(L"images/boss.png");
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
	curHeatlh -= a_dmg;
}

float Enemy::checkHealth()
{
	return curHeatlh;
}
	
void Enemy::update(float dT)
{
	// animation
	ghostAnim.update(dT);

	CoolDownCtrl(dT,atkTimer);
	CoolDownCtrl(dT,aniTimer);
	CoolDownCtrl(dT,ultTimer);
	bool attacking = false;
	// if not boss work normally
	static bool init = true;
	if(init)
	{
		playerPos = m_player->getPosition();
		findPath();
		init = false;
	}

	if(m_type != BOSS)
	{
		// check if player is in attack range
		if (m_player->getPosition().isWithin(atkRange, m_pos) && !atkTimer.active)
		{
			m_player->takeDmg(dmg);
			atkTimer.active = true;
			aniTimer.active = true;
			velocity = V2DF(0,0);
			attacking = true;
		}
		else if (!m_player->getPosition().isWithin(siteRange, m_pos))
		{
			if(playerPos != m_player->getPosition() || m_pos != path.get(path.size() -2)->getPosition())
			{
				findPath();
			}
			seek(path.get(path.size()-2)->getPosition());
		}
		else if(m_player->getPosition().isWithin(siteRange, m_pos)
			&& !m_player->getPosition().isWithin(atkRange, m_pos))
		{
			
			seek(m_player->getPosition());
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
		seek(m_player->getPosition());
		V2DF avoid(0,0);
		
	}

	// make sure velocity doesn't exceed max
	if (velocity.length() > mSpd * GRID_SIZE)
		velocity = velocity.normal().product(mSpd * GRID_SIZE);
	// update position
	
		m_pos.add( velocity.product(dT) );

	// degrade velocity by 20%
	velocity.multiply(.8);

	// determine facing
	if(attacking || velocity.isZero())
	{
		// attacking so face the player
		V2DF difference = m_player->getPosition().difference(m_pos);
		m_angle = atan2f(difference.y,difference.x)*(180.0f/V2D_PI) + 90.0f;
	}
	else
		// otherwise change facing based on velocity
		m_angle = atan2f(velocity.y,velocity.x)*(180.0f/V2D_PI) + 90.0f;

	// control attack animation
	if(aniTimer.active)
		m_tex.setTint(125,20,20);
	else
		m_tex.setTint(255,255,255);

	// check for collisions
	if( checkForCollisions() )
		// if any collisions occur resolve them
		resolveCollisions();
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
void Enemy::findPath()
{
	GraphNode* n = getMyPos();
	V2DF pos = this->m_pos;
	if(n)
	{
		pathFinder.findPath(n, m_room->getPlayerNode(m_player), &path);
	}
}
void Enemy::seek(V2DF target)
{
	// determine current steering force


	V2DF steeringForce = target.difference(m_pos);
	velocity.add(steeringForce);
}

bool Enemy::checkForCollisions()
{
	bool collided = false;
	// check against all walls
	collided = m_room->coll_walls(this);
	collided = collided | m_room->coll_enemies(this,0);
	collided = collided | m_player->collisionCheck(this, true);
	return collided;
}

GraphNode * Enemy::getMyPos()
{


	GraphNode * currentNode = m_room->getNode(m_pos);
	return currentNode;
}

void Enemy::render()
{
	//m_tex.draw(m_pos, m_angle, m_scale);
	ghostAnim.render(m_pos, m_angle, m_scale);
	// draw all nodes in the current path
	/*for(int i = 0; i < path.size(); ++i)
		m_nodeTex.draw( path.get(i)->getPosition(), 0.0f, 1.0f);*/
}