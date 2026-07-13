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

// BaseMenuPage.h
/// \file 
/// Defines the Base page that is loaded by the menu system when it is created.

#pragma once

#include "Page.h"

#include <AK/SoundEngine/Common/AkCallback.h>


/// This class defines the first page that the Menu system automatically loads
/// when the Menu class is instantiated.
class GMEBaseMenuPage : public Page
{
public:

	/// BaseMenuPage class constructor
	GMEBaseMenuPage( Menu* in_ParentMenu );

	/// Initializes the demo.
	/// \return True if successful and False otherwise.
	virtual bool Init();

	/// Releases resources used by the demo.
	virtual void Release();

	/// Override of the parent's update method.
	virtual bool Update();

	/// Registered global callback functions.
	static void SuspendResumeCallback(AK::IAkGlobalPluginContext* in_pContext, AkGlobalCallbackLocation in_eLocation, void* in_pCookie);

private:

	/// Initializes the various components on the page.
	virtual void InitControls();

	// Callback function management
	bool RegisterCallback();
	void ReleaseCallback();

	/// Delegate function for page navigation buttons.
	void OpenGMEVoiceChatPage(void* in_pSender, ControlEvent* in_pEvent);

	void OpenGMEVoiceMessagePage(void* in_pSender, ControlEvent* in_pEvent);
	
    void SetRegionButton_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);

};
