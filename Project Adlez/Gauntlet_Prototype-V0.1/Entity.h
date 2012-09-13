#pragma once
#include "v2d.h"
#include "Texture.h"
#include "HelpfulData.h"

// used as return type for collision function
// NONE = no collision occured
// MAIN = only the main rect was collided with or only the main rect was checked
// RIGHT, LEFT, TOP, BOT = corresponding side was collided 
enum SIDE {NONE,RIGHT,LEFT,TOP,BOT,MAIN};

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
public:
	// default constructor
	Entity()
		:m_angle(0), m_scale(1) 
	{}

	// Set up the base members of the entity class
	// ...and load the given image file into texture
	Entity(V2DF a_pos, LPCWSTR texName, FRect a_rect, float a_angle, float a_scale)
		:m_pos(a_pos), m_boundRect(a_rect), m_angle(a_angle), m_scale(a_scale)
	{ m_tex.initialize(texName); }

	// Same as constructor for ease of use
	void initialize(V2DF a_pos, LPCWSTR texName, FRect a_rect, float a_angle, float a_scale)
	{
		m_pos = a_pos;
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
	// ...calculate interpentration as well as move the entities apart
	// returns a SIDE to based on what occured NONE = no collision, MAIN = only the main
	// RIGHT, LEFT, TOP, BOT = the appropriate side was collided with
	// checkSides true = check sub rects, false = ignore
	// NOTE:: the entity making the function call is the one that will be moved, other will be left were it is...
	// ...this also means that only the calling entities sub rects will be taken into account
	SIDE collisionCheck(Entity * other, bool checkSides)
	{
		// this rect will be used only for interpentration calculations
		FRect mainA = getRelativeBoundRect();
		// first determine if the entites main rects are colliding
		// get the rects relative to positions
		FRect rectA = getRelativeBoundRect();
		FRect rectB = other->getRelativeBoundRect();
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
				rectB = other->getRelativeBoundRect();
				if( colliding(rectA, rectB) )
				{
					// refresh copies of main rects
					mainA = getRelativeBoundRect();
					// calculate interpenetration distance
					float pen = mainA.right - rectB.left;
					// move this entity accordingly
					m_pos.x -= pen;
					// set return value 
					temp = RIGHT;
				}
				// LEFT
				rectA = getSubRect(LEFT);
				rectB = other->getRelativeBoundRect();
				if( colliding(rectA, rectB) )
				{
					// refresh copies of main rects
					mainA = getRelativeBoundRect();
					// calculate interpenetration distance
					float pen = mainA.left - rectB.right;
					// move this entity accordingly
					m_pos.x -= pen;
					// set return value
					temp = LEFT;
				}
				// TOP
				rectA = getSubRect(TOP);
				rectB = other->getRelativeBoundRect();
				if( colliding(rectA, rectB) )
				{
					// refresh copies of main rects
					mainA = getRelativeBoundRect();
					// calculate interpenetration distance
					float pen = mainA.top - rectB.bottom;
					// move this entity accordingly
					m_pos.y -= pen;
					// set return value
					temp = TOP;
				}
				// BOTTOM
				rectA = getSubRect(BOT);
				rectB = other->getRelativeBoundRect();
				if( colliding(rectA, rectB) )
				{
					// refresh copies of main rects
					mainA = getRelativeBoundRect();	
					// calculate interpenetration distance
					float pen = mainA.bottom - rectB.top;
					// move this entity accordingly
					m_pos.y -= pen;
					// set return value
					temp = BOT;
				}
				// now return temp
				return temp;
			}
			else
				return MAIN;
		}
		// no collision occurd return none
		return NONE;
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
};