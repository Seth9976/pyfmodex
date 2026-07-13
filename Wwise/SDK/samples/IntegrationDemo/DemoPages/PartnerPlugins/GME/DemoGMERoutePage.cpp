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

#include"stdafx.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include "GMEDefines.h"
#ifdef INTDEMO_GME

#include "DemoGMERoutePage.h"
#include "Menu.h"

/////////////////////////////////////////////////////////////////////
// RoutePage Public Methods
/////////////////////////////////////////////////////////////////////

RoutePage::RoutePage( Menu* in_ParentMenu, AkUInt64 gameObjectID):Page(in_ParentMenu, "Routing")
{
	m_szHelp  = "This Page shows how to modify send route\n";
	m_gameObjectID = gameObjectID;
}

void RoutePage::InitControls()
{
	ToggleControl* newToggle = new ToggleControl(*this);
	newToggle->SetLabel("Send Route: ");
	newToggle->AddOption("Send Only");
	newToggle->AddOption("Send with Loopback");
	// Do something about that
	//newToggle->Select(GMEWWisePlugin_SendPlugin_GetEnableLoopbackWithGameObjectID(m_gameObjectID) ? 1 : 0);

	newToggle->SetDelegate((PageMFP)&RoutePage::GMERouteButton_Pressed);
	m_Controls.push_back(newToggle);

}

void RoutePage::GMERouteButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	ToggleControl* sender = (ToggleControl*)in_pSender;
	if (sender->SelectedIndex() == 0)
	{
		GMEWWisePlugin_SendPlugin_EnableLoopbackWithGameObjectID(m_gameObjectID, false);
	}
	else
	{
		GMEWWisePlugin_SendPlugin_EnableLoopbackWithGameObjectID(m_gameObjectID, true);
	}
}

#endif // INTDEMO_GME
