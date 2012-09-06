#include "DX2DEngine.h"
#include "Texture.h"
#include <stdlib.h>

// make all pointers zero
DX2DEngine::DX2DEngine()
{
	m_pD3DObject = 0;
	m_pD3DDevice = 0;
	m_pSprite = 0;
	m_bVsync = false;
	
	fps = 0;
	frameCount = 0;
	elapsedTime = 0.0;
}

// make sure the release is called
DX2DEngine::~DX2DEngine()
{
	release();
}

// set up direct x and other class components
void DX2DEngine::initialize(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
	m_hWnd = hWnd;

	// initialize fps, frameCount, and elapsed time to 0
	elapsedTime = 0.0f;
	fps = frameCount = 0;

	// Calculate RECT structure for text drawing placement, using whole screen
	RECT rect;
	GetClientRect(m_hWnd, &rect);

	// Create the D3D Object
	m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// calculate width/height of window
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Set D3D Device presentation parameters before creating the device
	D3DPRESENT_PARAMETERS D3Dpp;
	ZeroMemory(&D3Dpp, sizeof(D3Dpp));  // NULL the structure's memory

	D3Dpp.hDeviceWindow					= hWnd;										// Handle to the focus window
	D3Dpp.Windowed						= bWindowed;								// Windowed or Full-screen boolean
	D3Dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	D3Dpp.EnableAutoDepthStencil		= TRUE;										// Enables Z-Buffer (Depth Buffer)
	D3Dpp.BackBufferCount				= 1;										// Change if need of > 1 is required at a later date
	D3Dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;							// Back-buffer format, 8 bits for each pixel
	D3Dpp.BackBufferHeight				= height;									// Make sure resolution is supported, use adapter modes
	D3Dpp.BackBufferWidth				= width;									// (Same as above)
	D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;					// Discard back-buffer, must stay discard to support multi-sample
	D3Dpp.PresentationInterval			= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE; // Present back-buffer immediately, unless V-Sync is on								
	D3Dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;		// This flag should improve performance, if not set to NULL.
	D3Dpp.FullScreen_RefreshRateInHz	= bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;	// Full-screen refresh rate, use adapter modes or default
	D3Dpp.MultiSampleQuality			= 0;										// MSAA currently off, check documentation for support.
	D3Dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;						// MSAA currently off, check documentation for support.

	// Check device capabilities
	DWORD deviceBehaviorFlags = 0;
	m_pD3DObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps);

	// Determine vertex processing mode
	if(m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (Video Card)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	}
	
	// If hardware vertex processing is on, check pure device support
	if(m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;	
	}
	
	// Create the D3D Device with the present parameters and device flags above
	m_pD3DObject->CreateDevice(
		D3DADAPTER_DEFAULT,		// which adapter to use, set to primary
		D3DDEVTYPE_HAL,			// device type to use, set to hardware rasterization
		hWnd,					// handle to the focus window
		deviceBehaviorFlags,	// behavior flags
		&D3Dpp,					// presentation parameters
		&m_pD3DDevice);			// returned device pointer

	// Create a sprite object, note you will only need one for all 2D sprites
	D3DXCreateSprite( m_pD3DDevice, &m_pSprite );

	//////////////////////////////////////////////////////////////////////////
	// Create a Font Object
	//////////////////////////////////////////////////////////////////////////
	
	// Load a font for private use for this process
	AddFontResourceEx(L"Delicious-Roman.otf", FR_PRIVATE, 0);

	// Load D3DXFont, each font style you want to support will need an ID3DXFont
	D3DXCreateFont( m_pD3DDevice, 30, 0, FW_BOLD, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
					DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"), &m_pFont );
}

DX2DEngine* DX2DEngine::getInstance()
{
	static DX2DEngine instance;
	return &instance;
}

// Clear the screen and begin the scene
void DX2DEngine::start2DRender()
{
	// Make sure the device is working
	if(!m_pD3DDevice)
		return;

	// Clear the back buffer, call BeginScene()
	m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.4f, 0.8f, 1.0f), 1.0f, 0);
	m_pD3DDevice->BeginScene();
}

// end the entire scene
void DX2DEngine::end2DRender()
{
	m_pD3DDevice->EndScene();
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

// start the sprite object during render
void DX2DEngine::startSprite()
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);
}

// stop the sprite object during render
void DX2DEngine::endSprite()
{
	m_pSprite->End();
}

// return a reference to the d3d device
IDirect3DDevice9* DX2DEngine::deviceRef()
{
	return m_pD3DDevice;
}

// return a reference to the d3d device
ID3DXSprite* DX2DEngine::spriteRef()
{
	return m_pSprite;
}

ID3DXFont* DX2DEngine::fontRef()
{
	return m_pFont;
}

// safetly release all data used
void DX2DEngine::release()
{
	// release the sprite object
	if(m_pSprite)
		m_pSprite->Release();
	m_pSprite = 0;

	// release the d3d device
	if(m_pD3DDevice)
		m_pD3DDevice->Release();
	m_pD3DDevice = 0;

	// make sure to release all textures
	for(int i = 0; i < m_textures.size(); i++)
	{
		// if the texture exists release it
		if(m_textures.get(i)->m_pTexture)
			m_textures.get(i)->m_pTexture->Release();
	}

	// now release the vector itself
	m_textures.release();

	// release the d3d object
	if(m_pD3DObject)
		m_pD3DObject->Release();
	m_pD3DObject = 0;
}

void DX2DEngine::writeText(char * text, RECT bounds)
{
	m_pFont->DrawTextA(0, text, -1, &bounds, DT_TOP | DT_LEFT | DT_NOCLIP,
		D3DCOLOR_ARGB(255, 0, 0, 0) );
}

void DX2DEngine::writeCenterText(char * text, RECT bounds)
{
	m_pFont->DrawTextA(0, text, -1, &bounds, DT_CENTER | DT_VCENTER | DT_NOCLIP,
		D3DCOLOR_ARGB(255, 0, 0, 0) );
}

// Load the given texture from a file if it is not already loaded
// if the texture already exists simply return its id
// return its ID
int DX2DEngine::createTexture(LPCWSTR file)
{
	// first load up the texture from the given file
	TextureInfo *temp = new TextureInfo;

	D3DXCreateTextureFromFileEx(m_pD3DDevice, file, 0, 0, 0, 0, 
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
			D3DCOLOR_XRGB(255, 0, 255), &temp->m_imageInfo, 0, &temp->m_pTexture); 

	int oldSize = m_textures.size();

	// now attempt to add this file to the vector
	int index = m_textures.addNoDuplicates( temp );

	// check if the added texture was a duplicate 
	// if so release it
	if(index < oldSize)
	{
		temp->m_pTexture->Release();
	}

	// returnt the index to the texture
	return index;
}

// draw the given texture with the given attributes if it exists
void DX2DEngine::renderTexture(int index, V2DF pos, float scale, float angle)
{
	// make sure the texture exists
	if(index < m_textures.size())
	{
		// make sure the texture pointer itself isn't null
		if(m_textures.get(index)->m_pTexture)
		{
			// Scaling
			D3DXMATRIX scaleMat;
			D3DXMatrixScaling( &scaleMat, scale, scale, 1.0f ); // objects don't scale in the z layer

			// Rotation on Z axis, value in radians, converting from degrees
			D3DXMATRIX rotMat;
			D3DXMatrixRotationZ( &rotMat, D3DXToRadian(angle) );

			// Translation
			D3DXMATRIX transMat;
			D3DXMatrixTranslation( &transMat, pos.x, pos.y, 0.0 );

			// World matrix used to set transform
			D3DXMATRIX worldMat;
			// Multiply scale and rotation, store in scale
			// Multiply scale and translation, store in world
			worldMat = scaleMat * rotMat * transMat;

			// Set Transform
			m_pSprite->SetTransform( &worldMat );

			// draw the texture at its center
			HRESULT hr = m_pSprite->Draw( m_textures.get(index)->m_pTexture, NULL,
				&D3DXVECTOR3(m_textures.get(index)->m_imageInfo.Width * 0.5f, m_textures.get(index)->m_imageInfo.Height * 0.5f, 0.0f), 
				NULL, D3DCOLOR_ARGB(255, 255, 255, 255) );
		}
	}
}