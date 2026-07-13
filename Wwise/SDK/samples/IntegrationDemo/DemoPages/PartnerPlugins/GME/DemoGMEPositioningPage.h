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
#include "MovableChip.h"

class DemoGMEPositioningPage : public Page
{
public:

	/// DemoGMEPositioningPage class constructor.
	DemoGMEPositioningPage(
		Menu* in_pParentMenu,	///< - Pointer to the Menu that the page belongs to
		AkUInt64 in_gameobject
		);

	/// Override of the Parent's Init() method.
	virtual bool Init();

	/// Override of the Parent's Release() method.
	virtual void Release();

	/// Override of the Parent's Update() method.
	virtual bool Update();

	/// Override of the Parent's Draw() method.
	virtual void Draw();

	virtual bool OnPointerEvent( PointerEventType in_eType, int in_x, int in_y );

private:

	/// Initializes the controls on the page.
	virtual void InitControls();
	void UpdateGameObjPos();

	// Helpers

	float PixelsToAKPos_X(float in_X);
	float PixelsToAKPos_Y(float in_y);

	// Members

	MovableChip *m_pChip;

	float m_fGameObjectX;
	float m_fGameObjectZ;
	float m_fWidth;
	float m_fHeight;
	AkUInt64 m_gameObject;
};
