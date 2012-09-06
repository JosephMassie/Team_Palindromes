#pragma once
#include "DX2DEngine.h"
#include "v2d.h"

// contains the necessary information to store for a single d3d texture
// ...and draw it to the screen
class Texture
{
private:
	// graphics engine reference
	DX2DEngine* engine;
	// Texture reference
	int textureID;
	// texture variables
	IDirect3DTexture9*	m_pTexture;		// Texture Object for a sprite
	D3DXIMAGE_INFO		m_imageInfo;	// File details of a texture
	// color of the texture
	int m_R;
	int m_G;
	int m_B;
public:
	Texture();
	~Texture();
	void initialize(LPCWSTR file);
	void initialize(LPCWSTR file, int R, int G, int B);
	void setTint(int R, int G, int B);
	void release();
	void draw(V2DF pos, float rotAngle, float scale);
	void draw2(V2DF pos, float rotAngle, float scale);
};