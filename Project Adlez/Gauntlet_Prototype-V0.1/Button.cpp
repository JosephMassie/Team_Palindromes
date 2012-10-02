#include "Button.h"

Button::Button()
{
}

void Button::initialize(FRect a_rect, V2DF a_pos)
{
	theSound = Sound::getInstance();
	m_pos = a_pos;
	m_idleTex.initialize(L"images/button.png");
	m_hoverTex.initialize(L"images/hover.png");
	bRect.top = a_rect.top + m_pos.y;
	bRect.bottom = a_rect.bottom + m_pos.y;
	bRect.left = a_rect.left + m_pos.x;
	bRect.right = a_rect.right + m_pos.x;
	input = Input::getInstance();
	engine = DX2DEngine::getInstance();
	click = hover = selected = false;
	text = 0;
	length = 0;
}

void Button::update(float dT)
{
	// get mouse position
	V2DF mouse = input->getMousePos();

	// now see if the mouse is over the button
	if(mouse.x <= bRect.right && mouse.x >= bRect.left &&
		mouse.y <= bRect.bottom && mouse.y >= bRect.top )
	{
		hover = true;
		// see if the mouse has been clicked
		if(input->mouseButtonDown(0))
		{
			selected = true;
		}
		else if( selected && !input->mouseButtonDown(0) )
		{
			click = true;
			theSound->playSound(PRESS);
		}
	}
	else
	{
		// reset
		click = hover = selected = false;
	}
}

// copy the given string to button text field
// null terminator is assumed to be included in the size count
// adds an extra null terminator just incase
void Button::setText(char * a_text, int count)
{
	// only do this if text is not already set
	if(!text)
	{
		text = new char[count];
		length = count+1;
		for(int i = 0; i < count; i++)
		{
			text[i] = a_text[i];
		}
		text[length-1] = 0;
	}
}

void Button::render()
{
	if(hover)
		m_hoverTex.draw( m_pos, 0, 1.0f );
	else
		m_idleTex.draw( m_pos, 0, 1.0f );
}

void Button::drawText()
{
	engine->writeCenterText(text, fRectToRECT(bRect));
}

bool Button::clicked()
{
	return click;
}