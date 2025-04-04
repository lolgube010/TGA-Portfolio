#pragma once
#include <Windows.h>
#include <bitset>
#include "Windowsx.h"
//#include "CommonUtilities/Vector2.h"
//#include <WinUser.h>

// keycodes sourced from https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
enum class KeyCode
	// these are in hex but also that doesn't matter since they're enums. so whatevs. not gonna bother replacing them.
{
	// Mouse Buttons
	LeftMouseButton = 0x01,
	RightMouseButton = 0x02,
	MiddleMouseButton = 0x04,
	MouseButtonFour = 0x05,
	MouseButtonFive = 0x06,

	// Miscellaneous Keys
	Backspace = 0x08, Tab = 0x09, Clear = 0x0C, Enter = 0x0D,
	Shift = 0x10, Ctrl = 0x11, Alt = 0x12, Pause = 0x13,
	CapsLock = 0x14, Escape = 0x1B, Space = 0x20,

	// Arrow Keys
	LeftArrow = 0x25, UpArrow = 0x26, RightArrow = 0x27, DownArrow = 0x28,

	// Function Keys
	F1 = 0x70, F2 = 0x71, F3 = 0x72, F4 = 0x73,
	F5 = 0x74, F6 = 0x75, F7 = 0x76, F8 = 0x77,
	F9 = 0x78, F10 = 0x79, F11 = 0x7A, F12 = 0x7B,

	// Number Keys
	Key0 = 0x30, Key1 = 0x31, Key2 = 0x32, Key3 = 0x33,
	Key4 = 0x34, Key5 = 0x35, Key6 = 0x36, Key7 = 0x37,
	Key8 = 0x38, Key9 = 0x39,

	// Alphabetic Keys
	A = 0x41, B = 0x42, C = 0x43, D = 0x44, E = 0x45,
	F = 0x46, G = 0x47, H = 0x48, I = 0x49, J = 0x4A,
	K = 0x4B, L = 0x4C, M = 0x4D, N = 0x4E, O = 0x4F,
	P = 0x50, Q = 0x51, R = 0x52, S = 0x53, T = 0x54,
	U = 0x55, V = 0x56, W = 0x57, X = 0x58, Y = 0x59,
	Z = 0x5A,

	// Windows Keys
	LeftWindows = 0x5B, RightWindows = 0x5C,

	// Numpad Keys
	Numpad0 = 0x60, Numpad1 = 0x61, Numpad2 = 0x62, Numpad3 = 0x63,
	Numpad4 = 0x64, Numpad5 = 0x65, Numpad6 = 0x66, Numpad7 = 0x67,
	Numpad8 = 0x68, Numpad9 = 0x69, Multiply = 0x6A, Add = 0x6B,
	Separator = 0x6C, Subtract = 0x6D, Decimal = 0x6E, Divide = 0x6F,

	// Special Keys
	NumLock = 0x90, ScrollLock = 0x91,

	// OEM Specific Keys
	SemicolonColon = 0xBA, Plus = 0xBB, Comma = 0xBC, Minus = 0xBD,
	Period = 0xBE, SlashQuestionMark = 0xBF, Tilde = 0xC0,
	LeftSquareBracket = 0xDB, BackslashPipe = 0xDC,
	RightSquareBracket = 0xDD, QuoteDouble = 0xDE,

	// Other Special Keys
	Attn = 0xF6, CrSel = 0xF7, ExSel = 0xF8, EraseEOF = 0xF9,
	Play = 0xFA, Zoom = 0xFB, NoName = 0xFC, PA1 = 0xFD,
	OEMClear = 0xFE,

	RightControl = 0xA3,

	// EXTRA DO NOT USE UNLESS YOU KNOW WHAT YOU'RE DOING!!
	MouseMovementInputMapperDelta,
};

enum class InputType // used exclusively for inputmapper. ignore.
{
	Down,
	Held,
	Up,
	MouseDelta,
};

namespace CommonUtilities
{
	class InputManager
	{
	public:
		InputManager(HWND& aHwnd);

		void Update();
		bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);

		bool GetIsKeyDown(int aKeyCode) const; // key is pressed the current frame
		bool GetIsKeyDown(KeyCode aKeyCode) const; // key is pressed the current frame

		bool GetIsKeyHeld(int aKeyCode) const; // checks if the key is pressed and held down
		bool GetIsKeyHeld(KeyCode aKeyCode) const; // checks if the key is pressed and held down

		bool GetIsKeyUp(int aKeyCode) const; // key is released
		bool GetIsKeyUp(KeyCode aKeyCode) const; // key is released

		POINT GetMousePos() const;
		POINT GetMouseDelta() const;

		static POINT GetMouseAbsPos(); // did you know that the iowa state university also uses the abbreviation absPos
		float GetMouseScrollDelta() const;

		static void SetMousePos(long aXPos, long aYPos);

		static void ConfineCursorToWindow();
		static void ReleaseCursorFromWindow();

		static void ShowMouse();
		static void HideMouse();

		bool GetIsMouseLockedToCenter() const;

		// just run this once.
		void LockMouseToCenter();
		// just run this once
		void ReleaseMouseFromCenter();

		void ToggleMouseLock();

	private:
		void LockMouseToCenterHelper();

		std::bitset<256> myTentativeKeys;
		std::bitset<256> myCurrentKeys;
		std::bitset<256> myPreviousKeys;

		static HWND* myHwnd; // todo- think about the implications of having this be static

		POINT myTentativeMousePos;
		POINT myCurrentMousePos;
		POINT myPreviousMousePos;

		POINT myTentativeMouseDelta;
		POINT myMouseDelta;

		float myTentativeMouseWheelDelta;
		float myMouseWheelDelta;

		bool myMouseIsLockedToCenter = false;
	};

	inline HWND* InputManager::myHwnd = nullptr;

	inline InputManager::InputManager(HWND& aHwnd) : myTentativeMousePos(), myCurrentMousePos(), myPreviousMousePos(),
	                                                 myTentativeMouseDelta(), myMouseDelta(),
	                                                 myTentativeMouseWheelDelta(0),
	                                                 myMouseWheelDelta(0)
	{
		myHwnd = &aHwnd;
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

		RAWINPUTDEVICE rid[1]{};
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags = RIDEV_INPUTSINK;
		rid[0].hwndTarget = aHwnd;
		RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
	}

	inline void InputManager::Update()
	{
		myPreviousMousePos = myCurrentMousePos;
		myCurrentMousePos = myTentativeMousePos;

		myMouseDelta = myTentativeMouseDelta;
		myTentativeMouseDelta = {0, 0};

		myMouseWheelDelta = myTentativeMouseWheelDelta / abs(myTentativeMouseWheelDelta);
		myTentativeMouseWheelDelta = 0;

		myPreviousKeys = myCurrentKeys;
		myCurrentKeys = myTentativeKeys;

		if (myMouseIsLockedToCenter)
		{
			LockMouseToCenterHelper();
		}
	}

	inline bool InputManager::UpdateEvents(const UINT message, const WPARAM wParam, const LPARAM lParam)
	{
		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			myTentativeKeys[wParam] = true;
			return true;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			myTentativeKeys[wParam] = false;
			return true;

		case WM_LBUTTONDOWN:
			myTentativeKeys[VK_LBUTTON] = true;
			return true;

		case WM_LBUTTONUP:
			myTentativeKeys[VK_LBUTTON] = false;
			return true;

		case WM_RBUTTONDOWN:
			myTentativeKeys[VK_RBUTTON] = true;
			return true;

		case WM_RBUTTONUP:
			myTentativeKeys[VK_RBUTTON] = false;
			return true;

		case WM_MBUTTONDOWN:
			myTentativeKeys[VK_MBUTTON] = true;
			return true;

		case WM_MBUTTONUP:
			myTentativeKeys[VK_MBUTTON] = false;
			return true;
		case WM_XBUTTONDOWN:
			{
				GET_XBUTTON_WPARAM(wParam) == 1
					? myTentativeKeys[VK_XBUTTON1] = true
					: myTentativeKeys[VK_XBUTTON2] = true;
				return true;
			}

		case WM_XBUTTONUP:
			{
				GET_XBUTTON_WPARAM(wParam) == 1
					? myTentativeKeys[VK_XBUTTON1] = false
					: myTentativeKeys[VK_XBUTTON2] = false;
				return true;
			}

		case WM_MOUSEWHEEL:
			myTentativeMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			return true;

		// This is only used for when you want X/Y coordinates.
		// Reason being is that it's clunky to rely on for delta
		// movements of the mouse. ClipRect and SetMousePos all
		// cause their own problems for input which are easily
		// solved by registering for the raw HID data and listening
		// for WM_INPUT instead.
		case WM_MOUSEMOVE:
			{
				myTentativeMousePos.x = GET_X_LPARAM(lParam);
				myTentativeMousePos.y = GET_Y_LPARAM(lParam);

				return true;
			}

		// Handles mouse delta, used in 3D navigation etc.
		case WM_INPUT:
			// HAS TO BE SET UP USING RAWINPUTDEVICE AND RegisterRawInputDevices!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			{
				UINT dwSize = sizeof(RAWINPUT);
				static BYTE lpb[sizeof(RAWINPUT)];

				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

				if (const auto raw = (RAWINPUT*)lpb; raw->header.dwType == RIM_TYPEMOUSE)
				{
					myTentativeMouseDelta.x += raw->data.mouse.lLastX;
					myTentativeMouseDelta.y += raw->data.mouse.lLastY;
				}
				return true;
			}
		}

		return false;
	}

	inline bool InputManager::GetIsKeyDown(const int aKeyCode) const
	{
		return myCurrentKeys[aKeyCode] && !myPreviousKeys[aKeyCode];
	}

	inline bool InputManager::GetIsKeyDown(KeyCode aKeyCode) const
	{
		return myCurrentKeys[static_cast<int>(aKeyCode)] && !myPreviousKeys[static_cast<int>(aKeyCode)];
	}

	inline bool InputManager::GetIsKeyHeld(const int aKeyCode) const
	{
		return myCurrentKeys[aKeyCode] && myPreviousKeys[aKeyCode];
	}

	inline bool InputManager::GetIsKeyHeld(KeyCode aKeyCode) const
	{
		return myCurrentKeys[static_cast<int>(aKeyCode)] && myPreviousKeys[static_cast<int>(aKeyCode)];
	}

	inline bool InputManager::GetIsKeyUp(const int aKeyCode) const
	{
		return !myCurrentKeys[aKeyCode] && myPreviousKeys[aKeyCode];
	}

	inline bool InputManager::GetIsKeyUp(KeyCode aKeyCode) const
	{
		return !myCurrentKeys[static_cast<int>(aKeyCode)] && myPreviousKeys[static_cast<int>(aKeyCode)];
	}

	inline POINT InputManager::GetMousePos() const
	{
		return {myCurrentMousePos.x, myCurrentMousePos.y};
	}

	// YO, DOWNWARDS IS POSITIVE. UPWARDS IS NEGATIVE.
	inline POINT InputManager::GetMouseDelta() const // YO, DOWNWARDS IS POSITIVE. UPWARDS IS NEGATIVE.
	{
		return {myMouseDelta.x, myMouseDelta.y};
	}

	inline POINT InputManager::GetMouseAbsPos()
	{
		POINT cursorPoint;
		GetCursorPos(&cursorPoint);
		return cursorPoint;
	}

	inline float InputManager::GetMouseScrollDelta() const
	{
		return myMouseWheelDelta;
	}

	inline void InputManager::SetMousePos(const long aXPos, const long aYPos)
	{
		SetCursorPos(aXPos, aYPos);
	}

	inline void InputManager::ConfineCursorToWindow()
	{
		RECT clipRect;

		GetClientRect(*myHwnd, &clipRect);

		POINT upperLeft{};
		upperLeft.x = clipRect.left;
		upperLeft.y = clipRect.top;

		POINT lowerRight{};
		lowerRight.x = clipRect.right;
		lowerRight.y = clipRect.bottom;

		MapWindowPoints(*myHwnd, nullptr, &upperLeft, 1);
		MapWindowPoints(*myHwnd, nullptr, &lowerRight, 1);

		clipRect.left = upperLeft.x;
		clipRect.top = upperLeft.y;
		clipRect.right = lowerRight.x;
		clipRect.bottom = lowerRight.y;

		ClipCursor(&clipRect);
	}

	inline void InputManager::ReleaseCursorFromWindow()
	{
		ClipCursor(nullptr);
	}

	inline void InputManager::ShowMouse()
	{
		ShowCursor(true);
	}

	inline void InputManager::HideMouse()
	{
		ShowCursor(false);
	}

	inline bool InputManager::GetIsMouseLockedToCenter() const
	{
		return myMouseIsLockedToCenter;
	}

	inline void InputManager::LockMouseToCenter()
	{
		myMouseIsLockedToCenter = true;
		HideMouse();
	}

	inline void InputManager::ReleaseMouseFromCenter()
	{
		ShowMouse();
		myMouseIsLockedToCenter = false;
	}

	inline void InputManager::ToggleMouseLock()
	{
		if (myMouseIsLockedToCenter)
		{
			ReleaseMouseFromCenter();
		}
		else
		{
			LockMouseToCenter();
		}
	}

	inline void InputManager::LockMouseToCenterHelper()
	{
		RECT clientRect;
		GetClientRect(*myHwnd, &clientRect);
		POINT centerPoint = {(clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2};

		ClientToScreen(*myHwnd, &centerPoint);
		SetCursorPos(centerPoint.x, centerPoint.y);

		myPreviousMousePos = centerPoint;
	}
}
