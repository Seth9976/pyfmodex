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

// WindowsInputMgr.cpp
/// \file 
/// Defines the methods declared in WindowsInputMgr.h

#include "stdafx.h"

#if !defined(AK_INPUT_SDL)

#include "InputMgr.h"
#include "InputMgrScePadHelpers.h"
#include "Platform.h"

#include <map>
#include <algorithm>

#include <AK/Tools/Common/AkFNVHash.h>

// Manually-including InputMgrScePadHelpers.cpp since it is optionally available and not baked into the vcproj
#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
#include "ScePadHelpers/InputMgrScePadHelpers.cpp"
#endif

// Keyboard bindings
static std::map<char, UGButton> s_vkBindings = {
	{ VK_UP, UG_DPAD_UP },
	{ VK_DOWN, UG_DPAD_DOWN },
	{ VK_LEFT, UG_DPAD_LEFT },
	{ VK_RIGHT, UG_DPAD_RIGHT },
	{ VK_RETURN, UG_BUTTON1 },
	{ VK_ESCAPE, UG_BUTTON2 },
	{ VK_SHIFT, UG_BUTTON5 },
	{ VK_F1, UG_BUTTON7 },
	{ VK_MENU, UG_OS_BUTTON },
	{ VK_F2, UG_BUTTON11 },
	{ 'W' , UG_DPAD_UP },
	{ 'A', UG_DPAD_LEFT},
	{ 'S', UG_DPAD_DOWN },
	{ 'D', UG_DPAD_RIGHT },
	{ 'Q', UG_BUTTON3 },
	{ 'E', UG_BUTTON4 },
	{ 'R', UG_BUTTON6 },
	{ 'Z', UG_BUTTON9 },
	{ 'X', UG_BUTTON10 },
	{ 'C' , UG_BUTTON8 },
};

//////////////////////////////////////////////////////////////////////////
// Systems for managing the gameinput interactions

typedef HRESULT(WINAPI* _GameInputCreate)(IGameInput** gameInput);

WindowsInputMgr::WindowsInputMgr()
	: m_pUInput( NULL )
{
	for (int i = 0; i < MAX_SCEPAD; i++)
	{
		m_scePadHandles[i] = -1;
	}
}

WindowsInputMgr::~WindowsInputMgr()
{
}

void WindowsInputMgr::OnGameInputDevice(
	GameInputCallbackToken token,
	void* context,
	IGameInputDevice* in_pDevice,
	uint64_t,
	GameInputDeviceStatus currentStatus,
	GameInputDeviceStatus)
{
	auto* pThis = reinterpret_cast<WindowsInputMgr*>(context);

	if (currentStatus & GameInputDeviceConnected)
	{
		const GameInputDeviceInfo* pDeviceInfo = in_pDevice->GetDeviceInfo();

#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
		// VendorID of 0x54C is a scepad device, so don't handle it -- we'll find this through scepad library instead
		if (pDeviceInfo->vendorId == 0x54C)
		{
			return;
		}
#endif
		AK::FNVHash32 hash;
		// See AK::GetAppLocalDeviceIdHash in the Motion sink plug-in for reference implementation here -- This hash is what the Motion sink plug-in requires
		AkUInt32 uDeviceId = hash.Compute(pDeviceInfo->deviceId.value, APP_LOCAL_DEVICE_ID_SIZE);

		AkInt32 iPlayerIndex = 0;
		AkUInt32 uInputObjIdx = 0;
		// inputObjects is a sorted array on the playerIndex, so iterate through it until we find an available playerIndex
		for (; uInputObjIdx < pThis->m_InputObjects.size(); ++uInputObjIdx)
		{
			if (pThis->m_InputObjects[uInputObjIdx].uDeviceId == uDeviceId)
			{
				//This device ID is already connected, don't add it again.
				return;
			}
			else if (pThis->m_InputObjects[uInputObjIdx].iPlayerIndex == iPlayerIndex)
			{
				iPlayerIndex++;
			}
			else
			{
				break;
			}
		}

		InputObject newInput;
		newInput.pDevice = in_pDevice;
		newInput.iPlayerIndex = iPlayerIndex;
		newInput.uDeviceId = uDeviceId;
		newInput.iScePadHandle = -1;
		pThis->m_InputObjects.push_back(newInput);
		sort(pThis->m_InputObjects.begin(), pThis->m_InputObjects.end(), CompareInputObjects);
		pThis->m_pUInput->AddDevice(iPlayerIndex, UGDeviceType_GAMEPAD, uDeviceId);
	}
	else
	{
		for (AkUInt32 uInputIdx = 0; uInputIdx < pThis->m_InputObjects.size(); ++uInputIdx)
		{
			if (pThis->m_InputObjects[uInputIdx].pDevice == in_pDevice)
			{
				pThis->m_pUInput->RemoveDevice(pThis->m_InputObjects[uInputIdx].iPlayerIndex);
				pThis->m_InputObjects.erase(pThis->m_InputObjects.begin() + uInputIdx);
				break;
			}
		}
	}
}

bool WindowsInputMgr::Init(
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount
)
{
	InputObject newInputObject;
	m_pUInput = new UniversalInput;

	// Add the system keyboard as player 0
	newInputObject.iPlayerIndex = 0;
	newInputObject.iScePadHandle = -1;
	m_InputObjects.push_back(newInputObject);
	sort(m_InputObjects.begin(), m_InputObjects.end(), CompareInputObjects);
	m_pUInput->AddDevice(0, UGDeviceType_KEYBOARD);

#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
	// Initialize the scepad library (devices will be added as connections are made)
	InitScePad(m_scePadHandles);
#endif

#if defined (INTDEMO_GAMEINPUT_AVAILABLE)
	// load the gameinput library but don't keep the handle; we only need to initialize the GameInput device,
	// and the GameInput dll does not support unloading before processing termination, so we shouldn't try to FreeLibrary later
	if (HMODULE hGameInputDll = ::LoadLibrary(L"GameInput.dll"))
	{
		_GameInputCreate fnGameInputCreate = (_GameInputCreate) ::GetProcAddress(hGameInputDll, "GameInputCreate");

		if (fnGameInputCreate == nullptr)
		{
			goto gameInputCleanup;
		}

		if (FAILED(fnGameInputCreate(&m_pGameInput)))
		{
			goto gameInputCleanup;
		}

		// set up a dispatcher object so that we can handle callbacks on the mainthread
		if (FAILED(m_pGameInput->CreateDispatcher(&m_pGameInputDispatcher)))
		{
			goto gameInputCleanup;
		}

		// Register the device callback so that we can enumerate devices as they come in
		m_pGameInput->RegisterDeviceCallback(
			nullptr, // don't filter to specific devices
			GameInputKindController | GameInputKindGamepad, // listen for callbacks of controllers and gamepads
			GameInputDeviceConnected, // listen for callbacks of device connected/disconnected
			GameInputBlockingEnumeration, // have the initial enumeration be synchronous in nature so we can add output devices right away
			this,
			OnGameInputDevice, // the callback itself
			&m_gameInputCallbackToken
		);
	}

	return true;

gameInputCleanup:
	if (m_pGameInputDispatcher)
	{
		m_pGameInputDispatcher->Release();
		m_pGameInputDispatcher = nullptr;
	}
	if (m_pGameInput)
	{
		m_pGameInput->Release();
		m_pGameInput = nullptr;
	}

	return false;

#else
	return true;
#endif
}


UniversalInput* WindowsInputMgr::UniversalInputAdapter() const
{
	return m_pUInput;
}


void WindowsInputMgr::Release()
{
	if ( m_pUInput )
	{
		delete m_pUInput;
		m_pUInput = NULL;
	}
	m_InputObjects.clear();

#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
	TermScePad();
#endif

	if (m_pGameInputDispatcher)
	{
		m_pGameInputDispatcher->Release();
		m_pGameInputDispatcher = nullptr;
	}
	if (m_pGameInput)
	{
		m_pGameInput->UnregisterCallback(m_gameInputCallbackToken, 5000);
		m_pGameInput->Release();
		m_pGameInput = nullptr;
	}

}

void WindowsInputMgr::Update()
{
	if (m_pGameInputDispatcher)
	{
		m_pGameInputDispatcher->Dispatch(1000);
	}

#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
	UpdateScePadConnectivity();
#endif

	for (AkUInt32 uInputIdx = 0; uInputIdx < m_InputObjects.size(); ++uInputIdx)
	{
		InputObject& inputObject = m_InputObjects[uInputIdx];
		if (inputObject.iPlayerIndex == 0)
		{
			// 'it' is pointing to the keyboard device
			TranslateKeyboard(0);
		}
		else if (inputObject.iScePadHandle >= 0)
		{
			TranslateScePadInput(inputObject.iPlayerIndex, inputObject.iScePadHandle);
		}
		else if (inputObject.pDevice)
		{
			TranslateGameInput(inputObject.iPlayerIndex, inputObject.pDevice);
		}
	}
}

#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
void WindowsInputMgr::UpdateScePadConnectivity()
{
	// First check if we need to remove any devices which have disconnected or changed deviceId (e.g. padType has changed)
	for (AkUInt32 uInputObjIdx = 0; uInputObjIdx < m_InputObjects.size(); ++uInputObjIdx)
	{
		int padHandle = m_InputObjects[uInputObjIdx].iScePadHandle;
		if (padHandle < 0)
		{
			continue;
		}
		AkDeviceID oldDeviceId = m_InputObjects[uInputObjIdx].uDeviceId;
		AkDeviceID newDeviceId;
		GetScePadDeviceId(padHandle, newDeviceId);
		if (oldDeviceId != newDeviceId)
		{
			m_pUInput->RemoveDevice(m_InputObjects[uInputObjIdx].iPlayerIndex);
			m_InputObjects.erase(m_InputObjects.begin() + uInputObjIdx);
			uInputObjIdx--;
		}
	}

	// Then check if any devices should be connected
	for (int iPadHandleIdx = 0; iPadHandleIdx < MAX_SCEPAD; iPadHandleIdx++)
	{
		AkDeviceID resolvedId = 0;
		int iScePadHandle = m_scePadHandles[iPadHandleIdx];
		GetScePadDeviceId(iScePadHandle, resolvedId);
		if (resolvedId != 0)
		{
			// quick initial check to see if this padhandle is already registered
			{
				bool bFoundInputObject = false;
				for (AkUInt32 uInputObjIdx = 0; uInputObjIdx < m_InputObjects.size(); ++uInputObjIdx)
				{
					if (m_InputObjects[uInputObjIdx].iScePadHandle == iScePadHandle)
					{
						bFoundInputObject = true;
						break;
					}
				}
				if (bFoundInputObject)
				{
					continue;
				}
			}

			// Add the input object now
			AkInt32 iPlayerIndex = 0;
			AkUInt32 uInputObjIdx = 0;
			for (; uInputObjIdx < m_InputObjects.size(); ++uInputObjIdx)
			{
				if (m_InputObjects[uInputObjIdx].iPlayerIndex == iPlayerIndex)
				{
					iPlayerIndex++;
				}
				else
				{
					break;
				}
			}

			InputObject newInputObject;
			newInputObject.iPlayerIndex = iPlayerIndex;
			newInputObject.iScePadHandle = iScePadHandle;
			newInputObject.uDeviceId = resolvedId;
			m_pUInput->AddDevice((AkUInt16)newInputObject.iPlayerIndex, UGDeviceType_GAMEPAD, resolvedId);
			m_InputObjects.push_back(newInputObject);
			sort(m_InputObjects.begin(), m_InputObjects.end(), CompareInputObjects);

			iScePadHandle = -1;
		}
	}
}
#endif

void WindowsInputMgr::TranslateKeyboard(int in_iPlayerIndex)
{
	// Returned button state value
	UGBtnState iState = 0;
	UGStickState joysticks[2];
	memset(joysticks, 0, sizeof(joysticks));

	if (GetForegroundWindow() != m_hWnd)
	{
		//If window is out of focus, reset controls to zero to prevent stuck button inputs.
		m_pUInput->SetState((AkUInt16)0, true, iState, joysticks);
		return;
	}

#if defined(AK_WIN)
	if (m_bIsConsoleWindow)
	{
		DWORD cNumRead;
		INPUT_RECORD irInBuf[128];
		int counter = 0;

		// Get the standard input handle.

		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		if (hStdin == INVALID_HANDLE_VALUE)
			return;

		// Consume all console input events, if there are any
		while (GetNumberOfConsoleInputEvents(hStdin, &cNumRead) && cNumRead > 0)
		{
			if (ReadConsoleInput(
				hStdin,      // input buffer handle
				irInBuf,     // buffer to read into
				128,         // size of read buffer
				&cNumRead))
			{
				//Dispatch the events to the appropriate handler.
				for (int i = 0; i < (int)cNumRead; i++)
				{
					switch (irInBuf[i].EventType)
					{
					case KEY_EVENT: // keyboard input
						if (irInBuf[i].Event.KeyEvent.bKeyDown)
						{
							auto it = s_vkBindings.find((char)irInBuf[i].Event.KeyEvent.wVirtualKeyCode);
							if (it != s_vkBindings.end())
							{
								iState |= (*it).second;
							}
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}
	else
#endif
	{
		for (auto binding : s_vkBindings)
		{
			if (GetAsyncKeyState(binding.first) & 0x8000)
			{
				iState |= binding.second;
			}
		}
	}
	m_pUInput->SetState( (AkUInt16) 0, true, iState, joysticks );
}

void WindowsInputMgr::TranslateGameInput(int in_iPlayerIndex, IGameInputDevice* in_pDevice)
{
	UGBtnState btnState = 0;	// Returned button state value
	UGStickState joysticks[2];
	memset(joysticks, 0, sizeof(joysticks));

	IGameInputReading *pReading;
	if (SUCCEEDED(m_pGameInput->GetCurrentReading(GameInputKindGamepad, in_pDevice, &pReading)))
	{
		GameInputGamepadState pad;
		if (pReading->GetGamepadState(&pad))
		{
			if ((pad.buttons & GameInputGamepadA) != 0)
				btnState |= UG_BUTTON1;

			if ((pad.buttons & GameInputGamepadB) != 0)
				btnState |= UG_BUTTON2;

			if ((pad.buttons & GameInputGamepadX) != 0)
				btnState |= UG_BUTTON3;

			if ((pad.buttons & GameInputGamepadY) != 0)
				btnState |= UG_BUTTON4;

			if ((pad.buttons & GameInputGamepadLeftThumbstick) != 0)
				btnState |= UG_BUTTON9;

			if ((pad.buttons & GameInputGamepadRightThumbstick) != 0)
				btnState |= UG_BUTTON10;

			if ((pad.buttons & GameInputGamepadLeftShoulder) != 0)
				btnState |= UG_BUTTON5;

			if ((pad.buttons & GameInputGamepadRightShoulder) != 0)
				btnState |= UG_BUTTON6;

			if ((pad.buttons & GameInputGamepadView) != 0)
				btnState |= UG_BUTTON8;

			if ((pad.buttons & GameInputGamepadMenu) != 0)
				btnState |= UG_BUTTON7;

			if ((pad.buttons & GameInputGamepadDPadUp) != 0)
				btnState |= UG_DPAD_UP;

			if ((pad.buttons & GameInputGamepadDPadDown) != 0)
				btnState |= UG_DPAD_DOWN;

			if ((pad.buttons & GameInputGamepadDPadLeft) != 0)
				btnState |= UG_DPAD_LEFT;

			if ((pad.buttons & GameInputGamepadDPadRight) != 0)
				btnState |= UG_DPAD_RIGHT;
		}
		m_pUInput->SetState((AkUInt16)in_iPlayerIndex, true, btnState, joysticks);
	}
}

void WindowsInputMgr::TranslateScePadInput(int in_iPlayerIndex, int in_iScePadHandle)
{
	UGBtnState   iState = 0; // Returned button state value
	UGStickState joysticks[2];
	memset(joysticks, 0, sizeof(joysticks));
	bool bConnected = false;

#if defined(INTDEMO_LIBSCEPAD_SUPPORT)
	ReadScePadState(in_iScePadHandle, bConnected, iState, joysticks[0], joysticks[1]);
#endif

	m_pUInput->SetState((AkUInt16)in_iPlayerIndex, bConnected, iState, joysticks);
}

bool WindowsInputMgr::CompareInputObjects( WindowsInputMgr::InputObject in_sFirst, WindowsInputMgr::InputObject in_sSecond )
{
	return ( in_sFirst.iPlayerIndex < in_sSecond.iPlayerIndex );
}

#endif
