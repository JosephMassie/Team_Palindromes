#include "Bullet.h"
#include "Room.h"

// set pointers to 0
Bullet::Bullet()
{

}

// make sure safe release is called
Bullet::~Bullet()
{
	release();
}

// set up bullets original position and trajectory
void Bullet::initialize(V2DF a_trajectory, V2DF a_pos, Room* a_room, float a_dmg)
{
	m_room = a_room;
	m_dmg = a_dmg;
	trajectory = a_trajectory;
	m_pos = a_pos;
	m_scale = 1.0f;
	// 90 adds a 90deg fudge factor to correct for convience
	m_angle = atan2f(trajectory.y, trajectory.x) * 180.0f/V2D_PI + 90.0f;
	m_boundRect.left = m_boundRect.top = -3;
	m_boundRect.right = m_boundRect.bottom = 3;
	// load texture
	m_tex.initialize(L"bullet.png");
}

void Bullet::update(float dT)
{
}

// keep moving based on trajectory, time, and speed
// return true if the bullet collided this update
bool Bullet::Update(float dT)
{
	// first move foward then check for collisions
	m_pos.add( trajectory.normal().multiply(speed*TEX_SIZE*dT) );
	// calculate rect
	FRect temp = getBoundRect();
	V2DF tPOS(1,1);
	// now check for collisions with all objects in the room
	// walls/doors
	if( m_room->collImpassable(temp,m_pos,&tPOS,TOP) )
		return true;
	// enemies
	if( m_room->collEnemies(temp, m_dmg) )
		return true;
	// return false otherwise no collision occured
	return false;
}