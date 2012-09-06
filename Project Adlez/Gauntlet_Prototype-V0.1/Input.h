#pragma once
// Direct Input includes
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include "v2d.h"

// This class handles input for the entire game and is a singleton for ease of use
class Input
{
private:
	HWND m_hWnd;
	IDirectInput8 * m_pDInput;				// Direct Input Object
	IDirectInputDevice8 * m_pKeyboard;	// Keyboard Object
	IDirectInputDevice8 * m_pMouse;		// Mouse Object
	// keyboard states 
	bool keysLastUpdate[256]; // used to detect whether a given key was pressed last frame
	char mKeyboardState[256];
	// mouse states
	DIMOUSESTATE2 mMouseState;
	Input();
public:
	~Input();
	void initialize(HINSTANCE hInst, HWND hWnd);
	void pollDevices();
	void release();
	bool keyDown(int key);
	bool keyPressed(int key);
	bool mouseButtonDown(int button);
	V2DF getMousePos();
	static Input* getInstance();
};