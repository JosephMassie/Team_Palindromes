#pragma once
#include "v2d.h"
#include "Texture.h"
#include "HelpfulData.h"

// used as return type for collision function
// NONE = no collision occured
// MAIN = only the main rect was collided with or only the main rect was checked
// RIGHT, LEFT, TOP, BOT = corresponding side was collided 
enum SIDE {NONE,RIGHT,LEFT,TOP,BOT,MAIN};

// UI placement
#define UI_LEFT 640
#define UI_TOP 60

// UI data
// used to pass data in bulk from Player & UI
struct STATUS
{
	float health;
	int slot1;
	int slot2;
};

// used to save collisions that have occured
struct Collision
{
	FRect m_rect;
	SIDE m_side;
	float m_pen;
};

// base class for all in-game entities
class Entity
{
protected:
	V2DF m_pos;
	V2DF velocity;
	float m_angle;
	float m_scale;
	Texture m_tex;
	FRect m_boundRect;
	FRect m_right;
	FRect m_left;
	FRect m_top;
	FRect m_bottom;
	TemplateVector<Collision> m_collisions;
public:
	// default constructor
	Entity()
		:m_angle(0), m_scale(1) 
	{}

	// Set up the base members of the entity class
	// ...and load the given image file into texture
	Entity(V2DF a_pos, LPCWSTR texName, FRect a_rect, float a_angle, float a_scale)
		:m_pos(a_pos), m_boundRect(a_rect), m_angle(a_angle), m_scale(a_scale)
	{ 
		if(texName)
			m_tex.initialize(texName); 
	}

	// Same as constructor for ease of use
	void initialize(V2DF a_pos, LPCWSTR texName, FRect a_rect, float a_angle, float a_scale)
	{
		m_pos = a_pos;
		if(texName)
			m_tex.initialize(texName);
		m_boundRect = a_rect;
		m_angle = a_angle;
		m_scale = a_scale;
	}

	// default destructor
	// make sure safer release is called
	~Entity() { release(); }

	// safetly release texture data
	// can be set by child to be more complex
	virtual void release()	{ m_tex.release(); }

	// set position
	void setPosition(V2DF a_pos) { m_pos = a_pos; }

	// return position
	V2DF getPosition() { return m_pos; }

	// virtual update function to be set by child classes
	virtual void update(float dT) {}

	// virtual text rendering function to be set by child classes
	virtual void drawText() {}

	// renders the texture by default
	// m_pos must be in screen coordinates
	virtual void render() { m_tex.draw(m_pos, m_angle, m_scale); }

	// Detects if the given entity is colliding with this entity
	// If the checkSides flag is true it will detect which side the collision occurded on and ...
	// ...calculate interpentration as well as move the entities apart.
	// All collisions that occured this frame are stored in the m_collisions vector and are resovled by the...
	// ...resolveCollisions() function
	// returns a SIDE to based on what occured NONE = no collision, MAIN = only the main
	// RIGHT, LEFT, TOP, BOT = the appropriate side was collided with
	// checkSides true = check sub rects, false = ignore
	// NOTE:: the entity making the function call is the one that will be moved, other will be left were it is...
	// ...this also means that only the calling entities sub rects will be taken into account
	SIDE collisionCheck(Entity * other, bool checkSides)
	{
		// first determine if the entites main rects are colliding
		// get the rects relative to positions
		FRect rectA = getRelativeBoundRect();
		// get the relative rect of the other entity
		FRect rectB = other->getRelativeBoundRect();
		Collision col;
		// check if they are colliding
		if( colliding(rectA, rectB) )
		{
			// used to store return value
			SIDE temp = MAIN;
			// now determine if sides need to be checked
			if(checkSides)
			{
				// check each subRect in the order RIGHT, LEFT, TOP, BOTTOM
				// RIGHT
				rectA = getSubRect(RIGHT);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.right - rectB.left);
					col.m_side = RIGHT;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value 
					temp = RIGHT;
				}
				// LEFT
				rectA = getSubRect(LEFT);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.left - rectB.right);
					col.m_side = LEFT;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value
					temp = LEFT;
				}
				// TOP
				rectA = getSubRect(TOP);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.top - rectB.bottom);
					col.m_side = TOP;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value
					temp = TOP;
				}
				// BOTTOM
				rectA = getSubRect(BOT);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.bottom - rectB.top);
					col.m_side = BOT;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value
					temp = BOT;
				}
			}
			return temp;
		}
		// no collision occurd return none
		return NONE;
	}

	// overload of the previous function
	// functions exactly the same although it takes in a rect rather than another entity
	SIDE collisionCheck(FRect rectB, bool checkSides)
	{
		// first determine if the entites main rects are colliding
		// get the rects relative to positions
		FRect rectA = getRelativeBoundRect();
		Collision col;
		// check if they are colliding
		if( colliding(rectA, rectB) )
		{
			// used to store return value
			SIDE temp = MAIN;
			// now determine if sides need to be checked
			if(checkSides)
			{
				// check each subRect in the order RIGHT, LEFT, TOP, BOTTOM
				// RIGHT
				rectA = getSubRect(RIGHT);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.right - rectB.left);
					col.m_side = RIGHT;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value 
					temp = RIGHT;
				}
				// LEFT
				rectA = getSubRect(LEFT);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.left - rectB.right);
					col.m_side = LEFT;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value
					temp = LEFT;
				}
				// TOP
				rectA = getSubRect(TOP);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.top - rectB.bottom);
					col.m_side = TOP;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value
					temp = TOP;
				}
				// BOTTOM
				rectA = getSubRect(BOT);
				if( colliding(rectA, rectB) )
				{
					// calculate interpenetration distance
					col.m_pen = (rectA.bottom - rectB.top);
					col.m_side = BOT;
					col.m_rect = rectB;
					m_collisions.add(col);
					// set return value
					temp = BOT;
				}
			}
			return temp;
		}
		// no collision occurd return none
		return NONE;
	}

	// resovle all collisions saved in the m_collisions vector and then clears the vector
	void resolveCollisions()
	{
		// get a copy of this entity's bound rect 
		FRect rectA = getRelativeBoundRect();
		// used to hold the current collisions being resloved
		Collision bestVert; bestVert.m_pen = 0.0f; bestVert.m_rect = rectA; bestVert.m_side = MAIN;
		Collision bestHoriz; bestHoriz.m_pen = 0.0f; bestHoriz.m_rect = rectA; bestHoriz.m_side = MAIN;
		// only search through the list if there was more than 1 collision
		if(m_collisions.size() > 0)
		{
			//assume the first collision is the best
			if( m_collisions.get(0).m_side == RIGHT || m_collisions.get(0).m_side == LEFT )
				bestHoriz = m_collisions.get(0);
			else
				bestVert = m_collisions.get(0);

			// go through the list of collisions and find the closest horizantel and vertical components to the entity
			for(int i = 1; i < m_collisions.size(); ++i)
			{
				// first determine if this was a horizantal or vertical collision
				if ( m_collisions.get(i).m_side == RIGHT || m_collisions.get(i).m_side == LEFT )
				{
					// check against the best horizantal
					if( m_collisions.get(i).m_pen < bestHoriz.m_pen || bestHoriz.m_pen == 0)
						bestHoriz = m_collisions.get(i);
				}
				else
				{
					// check against the best vertical
					if( m_collisions.get(i).m_pen < bestVert.m_pen || bestVert.m_pen == 0)
						bestVert = m_collisions.get(i);
				}
			}

			// now augment the position based on these values
			m_pos.x -= bestHoriz.m_pen;
			m_pos.y -= bestVert.m_pen;

			// remember to clear the list of collisions
			m_collisions.clear();
		}
	}

	// return a bound rect relative to position
	FRect getRelativeBoundRect()
	{
		FRect temp;
		temp.top = m_pos.y + m_boundRect.top;
		temp.left = m_pos.x + m_boundRect.left;
		temp.right = m_pos.x + m_boundRect.right;
		temp.bottom = m_pos.y + m_boundRect.bottom;
		return temp;
	}

	// return a copy of the bound rect 
	FRect getBoundRect()
	{
		return m_boundRect;
	}
	
	// return a sub bound rect of the given side relative to position
	FRect getSubRect(SIDE side)
	{
		FRect temp;
		switch(side)
		{
		case RIGHT:
			temp.top = m_pos.y + m_right.top;
			temp.left = m_pos.x + m_right.left;
			temp.right = m_pos.x + m_right.right;
			temp.bottom = m_pos.y + m_right.bottom;
			break;
		case LEFT:
			temp.top = m_pos.y + m_left.top;
			temp.left = m_pos.x + m_left.left;
			temp.right = m_pos.x + m_left.right;
			temp.bottom = m_pos.y + m_left.bottom;
			break;
		case TOP:
			temp.top = m_pos.y + m_top.top;
			temp.left = m_pos.x + m_top.left;
			temp.right = m_pos.x + m_top.right;
			temp.bottom = m_pos.y + m_top.bottom;
			break;
		case BOT:
			temp.top = m_pos.y + m_bottom.top;
			temp.left = m_pos.x + m_bottom.left;
			temp.right = m_pos.x + m_bottom.right;
			temp.bottom = m_pos.y + m_bottom.bottom;
			break;
		}
		return temp;
	}

	float getAngle() { return m_angle; }
};