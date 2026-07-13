// GController.mm
// Copyright (C) 2023 Audiokinetic Inc
/// \file
/// Common implementation (iOS and Mac OS X) to support Game Controllerfor

#import <Foundation/Foundation.h>
#import "GControllerMgr.h"
#import <GameController/GameController.h>
#import "UniversalInput.h"

static const float kDeadzone = 0.2;

static UGButton ReadDpad(GCControllerButtonInput * element, UGButton uBtnMask)
{
	float value = element.value;
	BOOL pressed = element.pressed;
	return pressed && abs(value) > kDeadzone ? uBtnMask : 0;
}

#define BTN_NUM	(4)

@interface GControllerMgr_internal : NSObject
{
@protected
	GCControllerDirectionPad *m_dpad[UG_MAX_GAMEPADS];
	GCControllerButtonInput *m_button[BTN_NUM][UG_MAX_GAMEPADS];
	GCControllerDirectionPad *m_leftStick[UG_MAX_GAMEPADS];
	GCControllerDirectionPad *m_rightStick[UG_MAX_GAMEPADS];
	int m_maxIndex;
}
- (id)Init;
- (void)Term;
- (void)OnControllerConnectionChange:(NSNotification *)notification;
- (bool)IsConnected:(int)index;
- (int)NumControllers;
- (void)GetControllerState:(int)index btnState:(UGBtnState *)btnState stickState:(UGStickState *)stickState;
@end

@implementation GControllerMgr_internal
- (id)Init;
{
	if (self == [super init])
	{
		NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
		[center addObserver:self selector:@selector(OnControllerConnectionChange:) name:GCControllerDidConnectNotification object:nil];
		[center addObserver:self selector:@selector(OnControllerConnectionChange:) name:GCControllerDidDisconnectNotification object:nil];
		m_maxIndex = -1;
	}
	return self;
}

- (void)Term;
{
	NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
	[center removeObserver:self name:GCControllerDidConnectNotification object:nil];
	[center removeObserver:self name:GCControllerDidDisconnectNotification object:nil];
}

- (void)OnControllerConnectionChange:(NSNotification *)notification;
{
	unsigned int occupiedIndex = 0;

	if (GCController.controllers.count > 0)
	{
		//	setup playerIndex
		NSEnumerator *enumerater = [GCController.controllers objectEnumerator];
		m_maxIndex = -1;
		while (GCController *controller = [enumerater nextObject])
		{
			if (controller.playerIndex != GCControllerPlayerIndexUnset)
				occupiedIndex |= 1 << controller.playerIndex;
		}

		enumerater = [GCController.controllers objectEnumerator];
		while (GCController *controller = [enumerater nextObject])
		{
			if (controller.playerIndex == GCControllerPlayerIndexUnset)
			{	//	if found unassigned controller, assign free playerindex
				for (int i = GCControllerPlayerIndex1; i <= GCControllerPlayerIndex4; i++)
					if ((occupiedIndex & (1 << i)) == 0)
					{
						controller.playerIndex = (GCControllerPlayerIndex)i;
						occupiedIndex |= 1 << i;
						break;
					}
			}

			int i = (int)controller.playerIndex;
			if (m_maxIndex < i) m_maxIndex = i;

			if (m_dpad[i] == nil)
			{
				if (controller.extendedGamepad != nil)
				{
					m_dpad[i] = controller.extendedGamepad.dpad;
					m_button[0][i] = controller.extendedGamepad.buttonA;
					m_button[1][i] = controller.extendedGamepad.buttonB;
					m_button[2][i] = controller.extendedGamepad.buttonX;
					m_button[3][i] = controller.extendedGamepad.buttonY;
					m_leftStick[i] = controller.extendedGamepad.leftThumbstick;
					m_rightStick[i] = controller.extendedGamepad.rightThumbstick;
				}
				else if (@available(ios 16.1, tvOS 16.1, macOS 10.12, *))
				{
					if (controller.microGamepad != nil)
					{
						m_dpad[i] = controller.microGamepad.dpad;
						m_button[0][i] = controller.microGamepad.buttonA;
						m_button[1][i] = controller.microGamepad.buttonX;
						m_button[2][i] = nil;
						m_button[3][i] = nil;
						m_leftStick[i] = nil;
						m_rightStick[i] = nil;
					}
				}
			}
		}
	}
	else
	{
		m_maxIndex = -1;
	}
	for (int i = 0; i < UG_MAX_GAMEPADS; i++)
	{
		if (m_dpad[i] != nil && (occupiedIndex & (1 << i)) == 0)
		{
			m_dpad[i] = nil;
			for (int j = 0; j < BTN_NUM; j++) m_button[j][i] = nil;
			m_leftStick[i] = nil;
			m_rightStick[i] = nil;
		}
	}
}

- (bool)IsConnected:(int)index;
{
	NSEnumerator *enumerater = [GCController.controllers objectEnumerator];
	while (GCController *controller = [enumerater nextObject])
	{
		if (controller.playerIndex == index)
			return true;
	}
	return false;
}

- (int)NumControllers;
{
	return m_maxIndex + 1;
}

- (void)GetControllerState:(int)index btnState:(UGBtnState *)btnState stickState:(UGStickState *)stickState;
{
	*btnState = 0;
	memset(&stickState[0], 0, sizeof(stickState[0]));
	memset(&stickState[1], 0, sizeof(stickState[1]));
	if (m_dpad[index] == nil) return;

	const int btnMap[BTN_NUM] = {
		UG_BUTTON1,
		UG_BUTTON2,
		UG_BUTTON3,
		UG_BUTTON4
	};

	*btnState |= ReadDpad(m_dpad[index].left, UG_DPAD_LEFT);
	*btnState |= ReadDpad(m_dpad[index].right, UG_DPAD_RIGHT);
	*btnState |= ReadDpad(m_dpad[index].up, UG_DPAD_UP);
	*btnState |= ReadDpad(m_dpad[index].down, UG_DPAD_DOWN);
	for (int i = 0; i < BTN_NUM; i++)
		if (m_button[i][index] != nil && m_button[i][index].pressed) *btnState |= btnMap[i];

	if (m_leftStick[index] != nil)
	{
		stickState[0].x = m_leftStick[index].xAxis.value;
		stickState[0].y = m_leftStick[index].yAxis.value;
		stickState[1].x = m_rightStick[index].xAxis.value;
		stickState[1].y = m_rightStick[index].yAxis.value;
	}
}
@end

static GControllerMgr_internal *gcMgr = nil;

void GControllerMgr::Init()
{
	gcMgr = [[GControllerMgr_internal alloc ] Init];
}

void GControllerMgr::Term()
{
	[gcMgr Term];
	gcMgr = nil;
}

bool GControllerMgr::IsConnected(int index)
{
	return [gcMgr IsConnected:index];
}

int GControllerMgr::NumControllers()
{
	return [gcMgr NumControllers];
}

void GControllerMgr::GetControllerState(int index, UGBtnState *btnState, UGStickState *stickState)
{
	[gcMgr GetControllerState:index btnState:btnState stickState:stickState];
}
