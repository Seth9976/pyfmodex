/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

// InputMgr.h
/// \file 
/// Contains the declaration of the Windows Input Manager class.
/// This class handles the relaying of input to the Menu System.

#pragma once

#include <vector>

#include <GameInput.h>
#include "UniversalInput.h"
#include "IAkInputMgr.h"
#include "InputMgrScePadHelpers.h"

/// Windows-Specific Input Manager class that handles all input devices and relays input
/// data to the Menu system.
class WindowsInputMgr : public IAkInputMgr
{
public:
	/// InputMgr class constructor
	WindowsInputMgr();

	/// InputMgr class destructor
	~WindowsInputMgr();

	// Set the HWND to get for foreground state.
	// Call this before Init.
	// Specify bIsConsoleWindow == true if and only if the HWND is not owned by the current process.
	// It is OK to not call this or pass a NULL HWND. This will not fail initialization, but you may end up with no attached devices.
	inline void SetHwnd(HWND hWnd, bool bIsConsoleWindow)
	{
		m_hWnd = hWnd;
		m_bIsConsoleWindow = bIsConsoleWindow;
	}

	virtual bool Init(AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount) override;

	virtual UniversalInput* UniversalInputAdapter() const override;

	virtual void Update() override;

	virtual void Release() override;

	virtual bool ShouldQuit() const override { return false; }
	
	AkUInt32 GetControllerOutputId(int in_iPlayerIndex) const override { return 0; }

private:

	/// Represents an input device being tracked by the Input Manager.
	struct InputObject
	{
		IGameInputDevice* pDevice; ///<- ref to GameInput device for gamepad
		int iScePadHandle;         ///<- handle for scePad (-1 if not relevant)
		int iPlayerIndex;          ///<- The 1-based player index associated with the device.
		AkUInt32 uDeviceId;        ///<- identifier for deviceId

		InputObject()
			: pDevice(nullptr)
			, iScePadHandle(-1)
			, iPlayerIndex(-1)
			, uDeviceId(0)
		{}
	};

	/// Retrieves input state from a keyboard then translates it to a Button State 
	/// usable by the Universal Input class and sets it.
	void TranslateKeyboard( 
		int in_iPlayerIndex            ///< - 1-Based Player Index of the keyboard (should be 0)
		);

	/// Retrieves input state from an XInput device (eg. Xbox 360 controller) then
	/// translates it to a Button State usable by the Universal Input class and sets it.
	void TranslateGameInput( 
		int in_iPlayerIndex,           ///< - 1-Based Player Index of the gameinput device to be polled
		IGameInputDevice* in_pDevice   ///< - gameinput device 
	);

	/// Retrieves input state from an ScePad device (eg. DualSense) then
	/// translates it to a Button State usable by the Universal Input class and sets it.
	void TranslateScePadInput(
		int iPlayerIndex,              ///< - 1-Based Player Index of the XInput device to be polled
		int iScePadHandle              ///< - scePad handle
	);


#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
	void UpdateScePadConnectivity();
#endif

	/// Comparison function to compare InputObjects for the c++ sort function from the algorithm library.
	/// \return True if the first parameter preceeds the second when in ascending order and false otherwise.
	static bool CompareInputObjects( 
		InputObject in_sFirst,         ///< - The first Input Object
		InputObject in_sSecond         ///< - The second Input Object
		);

	static void CALLBACK OnGameInputDevice(
		GameInputCallbackToken,
		void* context,
		IGameInputDevice* device,
		uint64_t,
		GameInputDeviceStatus currentStatus,
		GameInputDeviceStatus);
	
	/// The application's main window handle
	HWND m_hWnd;

	/// State for GameInput
	IGameInput* m_pGameInput;
	IGameInputDispatcher* m_pGameInputDispatcher;
	GameInputCallbackToken m_gameInputCallbackToken;

	/// Pointer to the Universal Input object to interface with the Menu System
	/// \sa UniversalInput
	UniversalInput* m_pUInput;

	/// Container to hold the Input Objects tracked by the Input Manager
	std::vector<InputObject> m_InputObjects;

	/// The maximum number of ScePad devices
	static const int MAX_SCEPAD = 4;

	// a list of scePadHandles that we have not yet added as inputObjects
	int m_scePadHandles[MAX_SCEPAD];

	bool m_bIsConsoleWindow = false;

};