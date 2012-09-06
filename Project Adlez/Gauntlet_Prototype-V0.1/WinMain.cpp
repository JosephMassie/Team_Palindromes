//////////////////////////////////////////////////////////////////////////
// Name:	WinMain.h
// Date:	April 2nd, 2010
// Author:	Kyle Lauing [klauing@devry.edu] or [kylelauing@gmail.com]
// Purpose: This file is used to create a simple window application for
//			use with the DirectXFramework developed for the GSP 381 course
//			for DeVry University.
// Disclaimer:	
//			Copyright © 2010 by DeVry Educational Development Corporation.
//			All rights reserved.  No part of this work may be reproduced 
//			or used in any form or by any means – graphic, electronic, or 
//			mechanical, including photocopying, recording, Web distribution 
//			or information storage and retrieval systems – without the 
//			prior consent of DeVry Educational Development Corporation.
//////////////////////////////////////////////////////////////////////////
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// My Includes
#include "DX2DEngine.h"
#include "Input.h"
#include "GameEngine.h"
#include "Sound.h"
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
#include <iostream>
using namespace std;
#include <windows.h>
#define VC_EXTRALEAN
#include <DShow.h>
#pragma comment(lib, "strmiids.lib")

//////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE L"X Gauntlet"

HWND				g_hWnd;			// Handle to the window
HINSTANCE			g_hInstance;	// Handle to the application instance
bool				g_bWindowed;	// Boolean for windowed or full-screen

// Entry point for the game or application.
int WINAPI wWinMain(HINSTANCE hInstance,	// Handle to the application
				   HINSTANCE hPrevInstance,	// Handle to the previous app
				   LPTSTR lpCmdLine,		// Command line string
				   int lpCmdShow);			// Show window value

LRESULT CALLBACK WndProc(HWND hWnd,			// Handle to the window
						 UINT message,		// Incoming Message
						 WPARAM wparam,		// Message Info
						 LPARAM lparam);	// Message Info

void InitWindow(void)
{
	WNDCLASSEX wndClass;  
	ZeroMemory(&wndClass, sizeof(wndClass));

	// set up the window
	wndClass.cbSize			= sizeof(WNDCLASSEX);			// size of window structure
	wndClass.lpfnWndProc	= (WNDPROC)WndProc;				// message callback
	wndClass.lpszClassName	= WINDOW_TITLE;					// class name
	wndClass.hInstance		= g_hInstance;					// handle to the application
	wndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);	// default cursor
	wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOWFRAME);	// background brush

	// register a new type of window
	RegisterClassEx(&wndClass);

	g_hWnd = CreateWindow(
		WINDOW_TITLE, WINDOW_TITLE, 							// window class name and title
		g_bWindowed ? WS_OVERLAPPEDWINDOW | WS_VISIBLE:(WS_POPUP | WS_VISIBLE),// window style
		CW_USEDEFAULT, CW_USEDEFAULT,							// x and y coordinates
		SCREEN_WIDTH, SCREEN_HEIGHT,							// width and height of window
		NULL, NULL,												// parent window and menu
		g_hInstance,											// handle to application
		NULL);

	// Display the window
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	g_hInstance = hInstance;	// Store application handle
	g_bWindowed = true;			// Windowed mode or full-screen

	// Init the window
	InitWindow();

	// Use this msg structure to catch window messages
	MSG msg; 
	ZeroMemory(&msg, sizeof(msg));

	//**************************************************************
	// Setup for time calculations
	// calculate how many secs are equal to a count
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 curTimeStamp = 0;
	// get original timeStamp
	__int64 lastTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&lastTimeStamp);
	//**************************************************************

	// Create both Graphics and Game Engines
	DX2DEngine* DXEngine = DX2DEngine::getInstance();
	DXEngine->initialize(g_hWnd, hInstance, g_bWindowed);

	// Set up Input
	Input* DInput = Input::getInstance();
	DInput->initialize(hInstance, g_hWnd);

	// Set up Game Engine
	GameEngine Engine;
	Engine.initialize();

	// set up sound
	Sound* theSound = Sound::getInstance();
	theSound->initialize();

	// Main Windows/Game Loop
	while( msg.message != WM_QUIT && Engine.isRunning() )
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
		//*******************************************************************
		// Calculate time since last update/frame
		// get current time stamp
		QueryPerformanceCounter((LARGE_INTEGER*)&curTimeStamp);
		// calculate delta t , remember to convert from counts to seconds
		float deltaT = (curTimeStamp - lastTimeStamp) * secsPerCnt;
		// set lastTimeStampe to curTimeStamp
		lastTimeStamp = curTimeStamp;
		//*******************************************************************

		// ******************************************************************
		// Input must poll keyboard and mouse
		DInput->pollDevices();
		theSound->update();
		// Call Game Engine Update function
		Engine.update(deltaT);
		// Call Graphics Engine and Game Engine Render functions
		DXEngine->start2DRender();
		DXEngine->startSprite();
		// ............................
		// Here call the Game Engine's Render to render all game objects
		Engine.render();
		// ............................
		DXEngine->endSprite();
		Engine.drawText();
		DXEngine->end2DRender();
		// ******************************************************************		
	}

	// Release the Graphics Engine
	//test.release();
	DXEngine->release();

	// Release Input
	DInput->release();

	// Release the Game Engine
	Engine.release();

	// Unregister window
	UnregisterClass(WINDOW_TITLE, g_hInstance);

	// Return successful
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// attempt to handle your messages
	switch(message)
	{
		case (WM_PAINT):
		{
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		}		
		case(WM_DESTROY):
		{
			PostQuitMessage(0); 
			break;
		}
		case(WM_KEYDOWN):
		{
			switch(wparam)
			{
				// Escape Key will close the application
				// Remove/Modify this call when creating a real game with menus, etc.
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
					break;
				}
			}
		}
	}

	// pass to default handler
	return DefWindowProc(hWnd, message, wparam, lparam);
}
