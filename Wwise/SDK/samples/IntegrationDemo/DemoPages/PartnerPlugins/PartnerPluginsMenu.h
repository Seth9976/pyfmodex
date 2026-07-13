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

#pragma once

#include "Page.h"

class PartnerPluginsMenu : public Page
{
public:
	PartnerPluginsMenu(Menu* in_ParentMenu);

private:
	virtual void InitControls();

#ifdef AK_PLATFORM_SUPPORTS_GME
	/// Delegate function for the "GME Plug-in Demo" button.
	void OpenGMEDemoPage(void* in_pSender, ControlEvent* in_pEvent);
#endif

#ifdef AK_PLATFORM_SUPPORTS_SPEECH_ENGINE
	/// Delegate function for the "speechEngine" button.
	void OpenSpeechEngineDemoPage(void* in_pSender, ControlEvent* in_pEvent);
#endif
};
