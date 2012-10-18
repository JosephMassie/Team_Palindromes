#include "HelpfulData.h"
#include "Entity.h"

void seedRand()
{
	srand(time(NULL));
}

bool colliding(FRect r1, FRect r2)
{
	// check veritcal first
	if( (r1.top > r2.top && r1.top < r2.bottom) 
		|| (r1.bottom > r2.top && r1.bottom < r2.bottom) )
	{
		// check horizantally
		if( (r1.left > r2.left && r1.left < r2.right) 
			|| (r1.right > r2.left && r1.right < r2.right) )
		{
			return true;
		}
	}
	return false;
}

float randomFloat(float min, float max)
{
	// multiply by 1000 to increase accuracy since % only works with integers
	float temp = ( rand() % (int)(((max + 1) - min) * 1000.0f) ) + (min * 1000.0f);
	temp /= 1000.0f;
	return temp;
}

int randomInt(int min, int max)
{
	return ( rand() % ((max + 1) - min) ) + min;
}

// controls cooldowns/timers for all game objects
void CoolDownCtrl(float dT, ClDwn &a_cldwn)
{
	// check whether on cooldown or not
	if (a_cldwn.timePassed < a_cldwn.duration && a_cldwn.active)
	{
		// increment cooldown timer
		a_cldwn.timePassed += dT;
	} // check if cooldown is done and needs reseting
	else if (a_cldwn.timePassed >= a_cldwn.duration)
	{
		a_cldwn.timePassed = 0.0f;
		a_cldwn.active = false;
	}
}

RECT fRectToRECT(FRect a_rect)
{
	RECT temp;
	temp.top = a_rect.top;
	temp.bottom = a_rect.bottom;
	temp.left = a_rect.left;
	temp.right = a_rect.right;
	return temp;
}

bool colliding(V2DF point, FRect rect)
{
	if( point.x <= rect.right && point.x >= rect.left 
		&& point.y <= rect.bottom && point.y >= rect.top )
		return true;
	return false;
}