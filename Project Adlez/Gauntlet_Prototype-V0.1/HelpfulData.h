//
//		This header file is full of usual unversal functions, structs, and other includes
//		
#pragma once
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
#include "templatearray.h"
#include "templatevector.h"
#include "v2d.h"

#define GRID_SIZE 32.0f
#define HALF_GRID 16.0f
#define BORDER 32.0f

struct FRect
{
	float top;
	float bottom;
	float right;
	float left;
};

// used to control cool downs of actions
// cooldowns/timers are based on the active bool
// if active is false then the ability/action is useble
// otherwise it is not
// make sure to set active to true when the ability is used...
// ... to start the timer
struct ClDwn
{
	// cool down control variables
	float duration;
	float timePassed;
	bool active;
};

// seed the random number generator
// WARNING: ONLY CALL ONCE AT INITIALIZATION
void seedRand();

// returns true if the two rects are colliding
bool colliding(FRect r1, FRect r2);

// returns true if the points lies within the rect
bool colliding(V2DF point, FRect rect);

// returns a random float between the two given numbers
float randomFloat(float min, float max);

// returns a random int between the two given numbers
int randomInt(int min, int max);

// cool down function 
void CoolDownCtrl(float dT, ClDwn &a_cldwn);

// takes in an float rect and converts it to an equivelent windows rect
RECT fRectToRECT(FRect a_rect);