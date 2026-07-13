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

#include <AK/SoundEngine/Common/AkTypes.h>
#include "Page.h"
#include "Platform.h"

#ifdef INTEGRATIONDEMO_APP_LIFECYCLE

class DemoAppLifecycle : public Page
{
public:

	DemoAppLifecycle( Menu* in_ParentMenu );

	virtual ~DemoAppLifecycle();

	virtual bool Init();

	virtual void Release();

	virtual void Draw();
	
	void SetSuspendState(bool bSuspended) { m_bSuspended = bSuspended; }

private:

	virtual void InitControls();
	
	void ApplySettings_Pressed( void*, ControlEvent* in_pEvent );

	bool m_bSuspended;
};

#endif // INTEGRATIONDEMO_APP_LIFECYCLE
