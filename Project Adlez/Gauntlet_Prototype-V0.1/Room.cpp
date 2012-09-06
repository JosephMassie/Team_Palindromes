#include "Room.h"
#include "HelpfulData.h"
#include "v2d.h"
#include <fstream>
#include <stdio.h>
#include "GameEngine.h"
using namespace std;

// Default constructor
Room::Room()
{
	// set visited to false
	visited = false;
}

// Default destructor
// make sure safer release is called
Room::~Room()
{ 
	release(); 
}

// safetly release all data
void Room::release()
{
	m_floorTex.release();
	m_wallTex.release();
	m_doorTex.release();
	m_lockedTex.release();
}

// load all textures and set up the room
void Room::initialize(char* roomFile, LPCWSTR wall, LPCWSTR floor, LPCWSTR door, LPCWSTR locked, ROOM_TYPE type, GameEngine *ref)
{
	pGEngine = ref;
	// set room type
	m_type = type;
	// first load textures
	m_floorTex.initialize(floor);
	m_wallTex.initialize(wall);
	m_doorTex.initialize(door);
	m_lockedTex.initialize(locked);
	// create fstream opject to load map from file
	int y = 0;
	FILE* file;
	fopen_s(&file, roomFile, "r");
	char temp = 0;
	// generate room placing doors where appropriate
	for( int y = 0; y < ROOM_HEIGHT; ++y )
	{
		for(int x = 0; x < ROOM_WIDTH; ++x)
		{
			// get current line of from the file
			temp = fgetc(file);
			// check if a line has ended. if so make sure to grab the next char
			if(temp == '\n')
				temp = fgetc(file);
			// make sure temp is not eof
			if(temp == EOF)
				break;
			// now parse this line to create the room
			switch(temp)
			{
			case'.': // floor
				m_layout[y][x].m_tex = &m_floorTex;
				m_layout[y][x].m_type = FLOOR;
				break;
			case '#': // wall
				m_layout[y][x].m_tex = &m_wallTex;
				m_layout[y][x].m_type = WALL;
				break;
			case 'D': // door
				m_layout[y][x].m_door.locked = false;
				m_layout[y][x].m_type = DOOR;
				m_layout[y][x].m_tex = &m_doorTex;
				break;
			case -1:
				break;
			}
		}
		// make sure temp is not eof
		if(temp == EOF)
			break;
	}
	// make sure to close the file
	fclose(file);
}

// draw the room
// and all its contents
void Room::render()
{
	// draw each tile of the room
	for(int y = 0; y < ROOM_HEIGHT; y++)
	{
		for(int x = 0; x < ROOM_WIDTH; x++)
		{
			V2DF temp;
			temp.x = (x * GRID_SIZE) + BORDER + HALF_GRID;
			temp.y = (y * GRID_SIZE) + BORDER + HALF_GRID;
			m_layout[y][x].m_tex->draw(temp, 0.0f, 1.0f);
		}
	}
}

// check the given location. if it is enterable return true else return false
Node Room::checkLocation(int x, int y)
{
	// first check and make sure the given x and y are within bounds of the room if not return false
	if(x < 0 || x >= ROOM_WIDTH || y < 0 || y >= ROOM_HEIGHT)
	{
		Node temp;
		temp.m_type = BAD;
		return temp;
	}
	// if within the room return the node
	return m_layout[y][x];
}

void Room::setDoors(Room* a_doors[4])
{
	// find the north and south doors
	for(int x = 0; x < ROOM_WIDTH; x++)
	{
		// north door
		if (m_layout[0][x].m_type == DOOR)
		{
			m_layout[0][x].m_door.nextRoom = a_doors[NORTH];
			m_layout[0][x].m_door.posNew = V2DF(x, ROOM_HEIGHT - 2);
		}
		// sout door
		if (m_layout[ROOM_HEIGHT - 1][x].m_type == DOOR)
		{
			m_layout[ROOM_HEIGHT - 1][x].m_door.nextRoom = a_doors[SOUTH];
			m_layout[ROOM_HEIGHT - 1][x].m_door.posNew = V2DF(x, 1);
		}
	}
	// find the east and west doors
	for(int y = 0; y < ROOM_HEIGHT; y++)
	{
		// west door
		if (m_layout[y][0].m_type == DOOR)
		{
			m_layout[y][0].m_door.nextRoom = a_doors[WEST];
			m_layout[y][0].m_door.posNew = V2DF(ROOM_WIDTH - 2, y);
		}
		// east door
		if (m_layout[y][ROOM_WIDTH - 1].m_type == DOOR)
		{
			m_layout[y][ROOM_WIDTH - 1].m_door.nextRoom = a_doors[EAST];
			m_layout[y][ROOM_WIDTH - 1].m_door.posNew = V2DF(1, y);
		}
	}
}

void Room::enterRoom(Player* thePlayer)
{
}

void Room::update(float dT)
{
}

// check if the given rect collides with any walls or doors on the map
// pass is 0 it is the player
// otherwise pass in something else.
// this position is used as outp do not put in the player's position
bool Room::collImpassable(FRect a_rect, V2DF pos, V2DF* pen, SIDE side)
{
	// go through the entire room and check against each door/wall
	for(int y = 0; y < ROOM_HEIGHT; y++)
	{
		for(int x = 0; x < ROOM_WIDTH; x++)
		{
			// check if this is the player or somethin
			if(pen->x == 0)
			{
				// only check walls and doors
				if( m_layout[y][x].m_type == WALL || (m_layout[y][x].m_type == DOOR && m_layout[y][x].m_door.locked) )
				{
					// calculate this tile's rect
					V2DF temp;
					temp.x = (x * GRID_SIZE) + BORDER + HALF_GRID;
					temp.y = (y * GRID_SIZE) + BORDER + HALF_GRID;
					FRect bRect;
					bRect.top = temp.y - 16.0f;
					bRect.left = temp.x - 16.0f;
					bRect.right = temp.x + 16.0f;
					bRect.bottom = temp.y + 16.0f;
					// check if it is colliding with the given rect
					bool collided = colliding(a_rect, bRect);
					if(collided) // if it is return true
					{
						// assume collision widths are 8 and 16
						// get the distance between the positions
						switch(side)
						{
						case RIGHT:
							pen->x = (a_rect.right - bRect.left);
							break;
						case LEFT:
							pen->x = (a_rect.left - bRect.right);
							break;
						case TOP:
							pen->y = (a_rect.top - bRect.bottom);
							break;
						case BOT:
							pen->y = (a_rect.bottom - bRect.top);
							break;
						}
						return true;
					}
				}
			}
			else
			{
				// only check walls and doors
				if(m_layout[y][x].m_type != FLOOR)
				{
					// calculate this tile's rect
					V2DF temp;
					temp.x = (x * GRID_SIZE) + BORDER + HALF_GRID;
					temp.y = (y * GRID_SIZE) + BORDER + HALF_GRID;
					FRect bRect;
					bRect.top = temp.y - 16.0f;
					bRect.left = temp.x - 16.0f;
					bRect.right = temp.x + 16.0f;
					bRect.bottom = temp.y + 16.0f;
					// check if it is colliding with the given rect
					bool collided = colliding(a_rect, bRect);
					if(collided) // if it is return true
					{
						// assume collision widths are 8 and 16
						// get the distance between the positions
						switch(side)
						{
						case RIGHT:
							pen->x = (a_rect.right - bRect.left);
							break;
						case LEFT:
							pen->x = (a_rect.left - bRect.right);
							break;
						case TOP:
							pen->y = (a_rect.top - bRect.bottom);
							break;
						case BOT:
							pen->y = (a_rect.bottom - bRect.top);
							break;
						}
						return true;
					}
				}
			}
		}
	}
	// the given rect collides with no walls or doors return false
	return false;
}

Room* Room::collOpenDoors(FRect a_rect, V2DF& posNew)
{
	for(int y = 0; y < ROOM_HEIGHT; ++y)
	{
		for(int x = 0; x < ROOM_WIDTH; ++x)
		{
			// calculate this tile's rect
			V2DF temp;
			temp.x = (x * GRID_SIZE) + BORDER + HALF_GRID;
			temp.y = (y * GRID_SIZE) + BORDER + HALF_GRID;
			FRect bRect;
			bRect.top = temp.y - 16.0f;
			bRect.left = temp.x - 16.0f;
			bRect.right = temp.x + 16.0f;
			bRect.bottom = temp.y + 16.0f;
			// if this space is an unlocked door and the player is on it return true
			if(m_layout[y][x].m_type == DOOR && !m_layout[y][x].m_door.locked &&
				colliding(a_rect, bRect) )
			{
				posNew = m_layout[y][x].m_door.posNew;
				return m_layout[y][x].m_door.nextRoom;
			}
		}
	}
	return 0;
}