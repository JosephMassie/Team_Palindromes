#include "Spawner.h"
#include "Player.h"
#include "Room.h"

void Spawner::initialize(Room* a_room, Player* player, int floorNum, V2DF a_pos, V2DF rPOS)
{
	// set up base enitty variables
	m_pos = a_pos;
	m_scale = 1.0f;
	m_angle = 0.0f;
	// set up spawner's collision rect
	m_boundRect.top = -10;
	m_boundRect.left = -10;
	m_boundRect.right = 10;
	m_boundRect.bottom = 10;
	// setup health
	curHealth = maxHealth = 100 * floorNum;
	alive = true;
	// set player & room references
	m_room = a_room;
	thePlayer = player;
	// position in room space used for spawning
	m_roomPOS = rPOS;
	// now based on the current floor set the number of enemies to be spawned
	max_enemies = floorNum * 5;
	// randomly pick a type
	// ************************************************
	// TODO:: implement when there are more enemy types
	// for now just set to ghost
	spawnType = GHOST;
	// ************************************************
	// decide color of spawner based on enemy spawn type
	int r = 255, g = 255, b = 255;
	// load texture
	m_tex.initialize(L"spawner.png", r, g, b);
	// setup spawn cool down
	canSpawn.active = false;
	canSpawn.duration = 1.5f;
	canSpawn.timePassed = 0.0f;
}

// draw the spawner and all of its enemies
void Spawner::render()
{
	// only draw if alive
	if(alive)
		// first render the spawner
		m_tex.draw(m_pos, m_angle, m_scale);
	// now draw all enemies
	for(int i = 0; i < m_enemies.size(); i++)
		m_enemies.get(i)->render();
}

// update all enemies
// spawn new enemies if available
void Spawner::update(float dT)
{
	// only spawn new enemies if alive
	if(alive)
	{
		// if health is below 0 it is dead
		if(curHealth <= 0.0f)
			alive = false;
		// update cool downs
		CoolDownCtrl(dT, canSpawn);

		/// check on enemy count
		if(m_enemies.size() < max_enemies && !canSpawn.active)
		{
			// spawn a new enemy
			m_enemies.add( new Enemy );
			// generate a new position next to me that is free
			int x = 0, y = 0;
			do
			{
				x = randomInt(-1,1) + m_roomPOS.x;
				y = randomInt(-1,1) + m_roomPOS.y;
			}while( m_room->checkLocation(x,y).m_type != FLOOR || (x-m_roomPOS.x == 0 && y-m_roomPOS.y == 0) );
			// initialize that enemy with the given position
			m_enemies.get( m_enemies.size()-1 )->initialize(spawnType,thePlayer, 
				V2DF( (x * TEX_SIZE) + HALF_TEX + BORDER, (y * TEX_SIZE) + HALF_TEX + BORDER ), m_room);
			// start spawn cool down
			canSpawn.active = true;
		}
	}

	for(int i = 0; i < m_enemies.size(); ++i)
	{
		// check if any enemies are dead
		if(m_enemies.get(i)->checkHealth() <= 0.0f)
			m_enemies.removeFast(i);
		// incase the original enemy was killed make sure the next enemy exists and update them
		if(i < m_enemies.size())
			m_enemies.get(i)->update(dT);
	}
}

// check if given rect is within the rects of any enemies in the room
// return true if at least one enemy collides and apply damage to that enemies health
// this version takes in a float to use as output for interpenatration distance
bool Spawner::collEnemies(FRect a_rect, float a_dmg, V2DF pos, V2DF* pen, SIDE side)
{
	// check against all enemies in the room
	for(int i = 0; i < m_enemies.size(); i++)
	{
		// first calculate its bounding rect
		FRect eRect = m_enemies.get(i)->getBoundRect();
		if( colliding(a_rect, eRect) )
		{
			m_enemies.get(i)->takeDmg(a_dmg);
			//pos = m_enemies.get(i)->getPosition();
			// this enemy collides with the given rect
			// assume collision rects are both 8x8
			switch(side)
			{
			case RIGHT:
				pen->x = (a_rect.right - eRect.left);
				break;
			case LEFT:
				pen->x = (a_rect.left - eRect.right);
				break;
			case TOP:
				pen->y = (a_rect.top - eRect.bottom);
				break;
			case BOT:
				pen->y = (a_rect.bottom - eRect.top);
				break;
			}
			return true;
		}
	}
	// no collisions occured
	return false;
}

// check if given rect is within the rects of any enemies in the room
// return true if at least one enemy collides and apply damage to that enemies health
bool Spawner::collEnemies(FRect a_rect, float a_dmg)
{
	// check against all enemies in the room
	for(int i = 0; i < m_enemies.size(); i++)
	{
		// first calculate its bounding rect
		FRect eRect = m_enemies.get(i)->getBoundRect();
		if( colliding(a_rect, eRect) )
		{
			m_enemies.get(i)->takeDmg(a_dmg);
			// this enemy collides with the given rect
			return true;
		}
	}
	// no collisions occured
	return false;
}

void Spawner::takeDmg(float a_dmg)
{
	curHealth -= a_dmg;
}

float Spawner::checkHealth()
{
	return curHealth;
}

// true = alive, false = dead and no more enemies
bool Spawner::checkIfLiving()
{
	if(alive)
		return true;
	if(m_enemies.size() > 0)
		return true;
	return false;
}