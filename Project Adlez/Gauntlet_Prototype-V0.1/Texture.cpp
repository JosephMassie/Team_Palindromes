#include "Texture.h"

// default constructor setting initial values to 0
Texture::Texture()
{
	// set pointers to null
	engine = 0;
}

// make sure that safe release is called
Texture::~Texture()
{
	release();
}

void Texture::setTint(int R, int G, int B)
{
	m_R = R;
	m_G = G;
	m_B = B;
}

// setup texture object and load the given file
void Texture::initialize(LPCWSTR fileName)
{
	// first get an instance of the graphics engine
	if(!engine)
		engine = DX2DEngine::getInstance();

	// set color to white by default
	m_R = m_G = m_B = 255;

	// get the length of fileName
	int i = 0;
	while(fileName[i] != 0)
	{
		i++;
	}
	// increment one more time for the null terminator
	i++;

	// also load from graphics engine
	textureID = engine->createTexture(fileName, i);
}

// setup texture object and load the given file
void Texture::initialize(LPCWSTR fileName, int R, int G, int B)
{
	// first get an instance of the graphics engine
	if(!engine)
		engine = DX2DEngine::getInstance();

	// get the length of fileName
	int i = 0;
	while(fileName[i] != 0)
	{
		i++;
	}

	// also load from graphics engine
	textureID = engine->createTexture(fileName, i);

	// set color to the given color
	m_R = R;
	m_G = G;
	m_B = B;
}


// create a sprite sheet
// count = total number of sprites in sheet
// rows = number of rows in the sheet
// columns = number of columns in the sheet
// size = the uniform size of each sprite in the sheet
void Texture::initialize(LPCWSTR fileName, int count, int rows, int columns, int size)
{
	// first get an instance of the graphics engine
	if(!engine)
		engine = DX2DEngine::getInstance();

	// get the length of fileName
	int i = 0;
	while(fileName[i] != 0)
	{
		i++;
	}

	// also load from graphics engine
	textureID = engine->createTexture(fileName, i);

	// generate rects for the sheet
	// temp rect used to hold the data
	RECT temp;
	temp.top = 0;
	temp.left = 0;
	temp.right = size;
	temp.bottom = size;

	// control variable to stop generating rects when all are done
	bool done = false;

	for(int y = 0; y < rows && !done; ++y)
	{
		for(int x = 0; x < columns && !done; ++x)
		{
			// first make sure count hasn't been succeed
			if( (x+(columns*y)) >= count )
			{
				// all of the rects are finished exit the loops
				done = true;
				break;
			}
			// set up the rect
			temp.top = y * size;
			temp.bottom = (y + 1) * size;
			temp.left = x * size;
			temp.right = (x + 1) * size;
			
			// store in the rects list
			m_rects.add(temp);
		}
	}
}

// safetly release all data
void Texture::release()
{
	m_rects.release();
}

// draw the texture with the given coordinates
void Texture::draw(V2DF pos, float rotAngle, float scale)
{
	// pass in 0 for sprite sheet rect
	engine->renderTexture(textureID, pos, scale, rotAngle, 0, m_R, m_G, m_B);
}

// draws the given sprite from the sheet with the given coordinates
void Texture::draw(int index, V2DF pos, float rotAngle, float scale)
{
	if(index >= 0 && index < m_rects.size())
	{
		// get the sprite's rect
		RECT temp = m_rects.get(index);

		engine->renderTexture(textureID, pos, scale, rotAngle, &temp, m_R, m_G, m_B);
	}
	else // if the is a issue draw the whole texture instead
		engine->renderTexture(textureID, pos, scale, rotAngle, 0, m_R, m_G, m_B);
}

int Texture::getImageCount()
{
	return m_rects.size();
}