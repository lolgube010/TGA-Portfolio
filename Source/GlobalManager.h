
// NEVER INCLUDE THIS!!!!!!!!!
// NEVER INCLUDE THIS!!!!!!!!!
// NEVER INCLUDE THIS!!!!!!!!!

// TODO- PROPER ENGINE CLASS TO MANAGE THIS.

#pragma once
#include "Input.h"
#include "Timer.h"

inline void SetupGlobals(HWND* ahWnd)
{
	globalTimer = new CommonUtilities::Timer();
	globalInput = new CommonUtilities::InputManager(*ahWnd);
}

inline void UpdateGlobals()
{
	globalTimer->Update();
	globalInput->Update();
}

inline void DeleteGlobals()
{
	delete globalTimer;
	delete globalInput;
}