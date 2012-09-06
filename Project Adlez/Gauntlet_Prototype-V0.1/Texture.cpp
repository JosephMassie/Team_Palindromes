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

// safetly release all data
void Texture::release()
{
}

// draw the texture with the given coordinates
void Texture::draw(V2DF pos, float rotAngle, float scale)
{
	engine->renderTexture(textureID, pos, scale, rotAngle);
}