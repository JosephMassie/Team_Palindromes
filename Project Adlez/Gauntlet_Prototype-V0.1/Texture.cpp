#include "Texture.h"

// default constructor setting initial values to 0
Texture::Texture()
{
	// set pointers to null
	engine = 0;
	m_pTexture = 0;
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

	// now create a texture fromt the given file
	if(!m_pTexture)
	{
		IDirect3DDevice9* device = engine->deviceRef();

		D3DXCreateTextureFromFileEx(device, fileName, 0, 0, 0, 0, 
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
			D3DCOLOR_XRGB(255, 0, 255), &m_imageInfo, 0, &m_pTexture);
	}
	// set color to white by default
	m_R = m_G = m_B = 255;

	// also load from graphics engine
	textureID = engine->createTexture(fileName);
}

// setup texture object and load the given file
void Texture::initialize(LPCWSTR fileName, int R, int G, int B)
{
	// first get an instance of the graphics engine
	if(!engine)
		engine = DX2DEngine::getInstance();

	// now create a texture fromt the given file
	if(!m_pTexture)
	{
		IDirect3DDevice9* device = engine->deviceRef();

		D3DXCreateTextureFromFileEx(device, fileName, 0, 0, 0, 0, 
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
			D3DCOLOR_XRGB(255, 0, 255), &m_imageInfo, 0, &m_pTexture);
	}
	// set color to the given color
	m_R = R;
	m_G = G;
	m_B = B;
}

// safetly release all data
void Texture::release()
{
	if(m_pTexture)
		m_pTexture->Release();
	m_pTexture = 0;
}

// draws the texture at a given positon with the given rotation
void Texture::draw2(V2DF pos, float rotAngle, float scale)
{
	// make sure there is a texture to draw
	if(!m_pTexture)
		return;

	// Scaling
	D3DXMATRIX scaleMat;
	D3DXMatrixScaling( &scaleMat, scale, scale, 1.0f ); // objects don't scale in the z layer

	// Rotation on Z axis, value in radians, converting from degrees
	D3DXMATRIX rotMat;
	D3DXMatrixRotationZ( &rotMat, D3DXToRadian(rotAngle) );

	// Translation
	D3DXMATRIX transMat;
	D3DXMatrixTranslation( &transMat, pos.x, pos.y, 0.0 );

	// World matrix used to set transform
	D3DXMATRIX worldMat;
	// Multiply scale and rotation, store in scale
	// Multiply scale and translation, store in world
	worldMat = scaleMat * rotMat * transMat;

	// Set Transform
	ID3DXSprite* sprite = engine->spriteRef();
	sprite->SetTransform( &worldMat );

	// draw the texture at its center
	HRESULT hr = sprite->Draw( m_pTexture, NULL,
		&D3DXVECTOR3(m_imageInfo.Width * 0.5f, m_imageInfo.Height * 0.5f, 0.0f), 
		NULL, D3DCOLOR_ARGB(255, m_R, m_G, m_B) );
}

// draw the texture with the given coordinates
void Texture::draw(V2DF pos, float rotAngle, float scale)
{
	engine->renderTexture(textureID, pos, scale, rotAngle);
}