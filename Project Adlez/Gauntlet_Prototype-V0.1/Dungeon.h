#pragma once
#include "Room.h"
#define FLR_SIZE 10

extern class GameEngine;
extern class Player;

// file parsing values
// file order NSEW.txt
// or a _ in a given place such as...
// N_EW.txt has no south door
#define LEN		16
#define BLANK	'_'
#define N_DOOR	'N'
#define S_DOOR	'S'
#define E_DOOR	'E'
#define W_DOOR	'W'

class Dungeon
{
private:
	Room* m_layout[FLR_SIZE][FLR_SIZE];
	Room* start;
	Room* end;
public:
	Dungeon();
	~Dungeon();
	void initialise(char* floorFile, GameEngine* ref, Player * thePlayer);
	void release();
	Room* getStart();
};