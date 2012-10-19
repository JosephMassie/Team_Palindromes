#include "Animation.h"

Animation::Animation() 
{
	m_texture = 0;
	currentFrame = 0;
}

Animation::~Animation() {}

void Animation::initialize(Texture * a_texture, float fps)
{
	begining = true;
	play = true;
	m_texture = a_texture;
	int numFrames = m_texture->getImageCount();
	currentFrame = 0;
	// setup animation timer
	m_frameSwitch.active = false;
	m_frameSwitch.duration = numFrames / fps;
	m_frameSwitch.timePassed = 0.0f;
}

// render the current frame and the given position, scale, and angle
void Animation::render(V2DF position, float angle, float scale)
{
	m_texture->draw(currentFrame, position, angle, scale);
}

// update the animation timer 
void Animation::update(float dT)
{
	CoolDownCtrl(dT, m_frameSwitch);

	if(play)
	{
		// check if it is time to switch frames
		if(!m_frameSwitch.active)
		{
			m_frameSwitch.active = true;
			m_frameSwitch.timePassed = 0.0f;
			// increment current frame
			if(!begining) // make sure that this is not the first time the animation is run
			{
				currentFrame++;
				// cycle back to the begining
				if(currentFrame >= m_texture->getImageCount())
					currentFrame = 0;
			}
			else
				begining = false;
		}
	}
}

// if reset is true start the animation from the begining if paused
// if reset is false just continue the animation
void Animation::start(bool reset)
{
	play = true;
	m_frameSwitch.active = true;
	if(reset)
	{
		begining = true;
		currentFrame = 0;
		m_frameSwitch.timePassed = 0.0f;
	}
}

// if reset is true stop the animation and set to starting frame
// if reset is false stop the animation where it is
void Animation::stop(bool reset)
{
	play = false;
	m_frameSwitch.active = false;
	m_frameSwitch.timePassed = 0.0f;
	if(reset)
	{
		begining = true;
		currentFrame = 0;
	}
}