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

class RoutePage : public Page
{
public:

	/// RoutePage class constructor
	RoutePage( Menu* in_ParentMenu, AkUInt64 gameObjectID);

private:

	/// Initializes the various components on the page.
	virtual void InitControls();

	void GMERouteButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	AkUInt64 m_gameObjectID;
};
