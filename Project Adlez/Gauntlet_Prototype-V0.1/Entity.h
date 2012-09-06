#pragma once
#include "v2d.h"
#include "Texture.h"
#include "HelpfulData.h"

enum SIDE {RIGHT,LEFT,TOP,BOT};

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

	// return a bound rect relative to position
	FRect getBoundRect()
	{
		FRect temp;
		temp.top = m_pos.y + m_boundRect.top;
		temp.left = m_pos.x + m_boundRect.left;
		temp.right = m_pos.x + m_boundRect.right;
		temp.bottom = m_pos.y + m_boundRect.bottom;
		return temp;
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