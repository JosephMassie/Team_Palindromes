#pragma once
///////////////////////////////////////////////////////////////////////////
// General Windows includes
///////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#pragma comment(lib, "winmm.lib")
//////////////////////////////////////////////////////////////////////////
// Direct3D 9 headers and libraries required
//////////////////////////////////////////////////////////////////////////
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
// used for windows error text box
#include <WinUser.h>
#include "v2d.h"
#include "templatearray.h"
#include "templatevector.h"

// texture struct
// saves the need for two lists of image info and images themselves
struct TextureInfo
{
	int					length;			// file name length
	char*				name;			// File name
	IDirect3DTexture9*	m_pTexture;		// Texture Object for a sprite
	D3DXIMAGE_INFO		m_imageInfo;	// File details of a texture
};

// Singleton for basic graphics use
// Simply an in-between making drawing 2d sprites in direct x easier
class DX2DEngine
{
private:
	// Application variables
	HWND m_hWnd;
	bool m_bVsync;
	// Direct X objects/devices
	IDirect3D9*			m_pD3DObject;	// Direct3D 9 Object
	IDirect3DDevice9*	m_pD3DDevice;	// Direct3D 9 Device
	D3DCAPS9			m_D3DCaps;		// Device Capabilities
	// Direct X sprites variable
	ID3DXSprite*		m_pSprite;		// Sprite Object
	// Direct X font variable
	ID3DXFont*			m_pFont;		// Font Object

	// Vector of currently loaded textures
	TemplateVector<TextureInfo*> m_textures;

	// frames per second
	int fps;
	// current number of frames rendered
	int frameCount;
	// elapsed time
	// used to calculate fps
	float elapsedTime;

	DX2DEngine();
public:
	// basic functions
	~DX2DEngine();
	static DX2DEngine* getInstance();
	void initialize(HWND& hWnd, HINSTANCE& hInst, bool bWindowed);
	void release();
	// render controlers
	void start2DRender();
	void end2DRender();
	void startSprite();
	void endSprite();
	// accessors
	ID3DXSprite* spriteRef();
	ID3DXFont* fontRef();
	IDirect3DDevice9* deviceRef();
	// Text drawing/writing functions
	void writeText(char *text, V2DF pos, float width, float height);
	void writeText(char *text, RECT bounds);
	void writeCenterText(char *text, RECT bounds);
	// Texture creation functions
	int createTexture(LPCWSTR file, int length);
	// Texture rendering functions
	void renderTexture(int index, V2DF pos, float scale, float angle, RECT *m_sheetRect);
};