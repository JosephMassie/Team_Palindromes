#include "Input.h"

// Return a reference to the input device
Input::Input()
{
	// set devices to null
	m_pDInput = 0;
	m_pKeyboard = 0;
	m_pMouse = 0;
}

Input* Input::getInstance()
{
	static Input instance;
	return &instance;
}

Input::~Input()
{
	release();
}

void Input::initialize(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	// Create dInput Object
	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, 0);

	// Create Mouse and Keyboard Objects
	m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, 0);
	m_pDInput->CreateDevice(GUID_SysMouse, &m_pMouse, 0);

	// Set Mouse and Keyboard Data Formats
	m_pKeyboard->SetDataFormat( &c_dfDIKeyboard);
	m_pMouse->SetDataFormat(&c_dfDIMouse2);

	// Set the Cooperative Level of both the Mouse and Keyboard
	m_pKeyboard->SetCooperativeLevel( hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	m_pMouse->SetCooperativeLevel( hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );

	// Aquire both the Mouse and Keyboard
	m_pKeyboard->Acquire();
	m_pMouse->Acquire();
}

void Input::pollDevices()
{
	///////////////////////////////////////////
	// Poll Keyboard & Mouse for Input
	///////////////////////////////////////////
	// Create Temp structures to hold Keyboard & Mouse states
	// Keyboard
	for(int i = 0; i < 256; i++)
		mKeyboardState[i] = 0; // refresh key input
	// Mouse
	mMouseState.lX = 0; // refresh mouse state
	mMouseState.lY = 0;
	mMouseState.lZ = 0;
	mMouseState.rgbButtons[0] = 0;
	mMouseState.rgbButtons[1] = 0;
	mMouseState.rgbButtons[2] = 0;

	// Poll the Keyboard
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(mKeyboardState), (void**)mKeyboardState);

	// refesh pressed buttons
	for(int i = 0; i < 256; ++i)
	{
		if( (!(mKeyboardState[i] & 0x80)) && keysLastUpdate[i] )
			keysLastUpdate[i] = false;
	}

	// make sure this call was successful
	if( FAILED(hr) )
	{
		// keyboard lost, zero out keyboard data structure
		ZeroMemory(mKeyboardState, sizeof(mKeyboardState));
		// Try to re-acquire for next time we poll
		m_pKeyboard->Acquire();
	}

	// Poll the mouse
	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mMouseState);

	// make sure this call was successful
	if( FAILED(hr) )
	{
		// keyboard lost, zero out keyboard data structure
		ZeroMemory(&mMouseState, sizeof(mMouseState));
		// Try to re-acquire for next time we poll
		m_pMouse->Acquire();
	}
}

bool Input::keyDown(int key)
{
	return (mKeyboardState[key] & 0x80);
}

// only valid input is 0,1, or 2
bool Input::mouseButtonDown(int button)
{
	return (mMouseState.rgbButtons[button] & 0x80);
}

bool Input::keyPressed(int key)
{
	if( mKeyboardState[key] & 0x80 && !keysLastUpdate[key] )
	{
		keysLastUpdate[key] = true;
		return true;
	}
	return false;
}

V2DF Input::getMousePos()
{
	POINT p;
	V2DF v;
	GetCursorPos(&p);
	ScreenToClient(m_hWnd, &p);
	v.x = p.x;
	v.y = p.y;
	return v;
}

void Input::release()
{
	// Keyboard & Mouse
	// safe release all created direct input objects
	m_pKeyboard->Unacquire();
	m_pMouse->Unacquire();
	if (!m_pKeyboard)
		m_pKeyboard->Release();
	if (!m_pMouse)
		m_pMouse->Release();
	if (!m_pDInput)
		m_pDInput->Release();
}