/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

// IntegrationDemoAppDelegate.mm
/// \file
/// Application delegate for the demo.

#import "IntegrationDemoAppDelegate.h"
#include "IntegrationDemo.h"
#include "Helpers.h"

#include "SDLWindow.h"

#include "AppleInputMgr.h"
AppleInputMgr g_InputMgr;

#include <SDL_syswm.h>

@interface IntegrationDemoAppDelegate(PrivateMethods)
-(void) AddObservers;
@end

@implementation IntegrationDemoAppDelegate
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	AkOSChar szError[500];

	AkInitSettings& initSettings = IntegrationDemo::Instance().GetInitSettings();
	initSettings.szPluginDLLPath = const_cast<char*>(DLL_PATH);
	g_uSamplesPerFrame = initSettings.uNumSamplesPerFrame;
	IntegrationDemo::Instance().GetPlatformInitSettings().bVerboseSystemOutput = true;
	
	SDL_Window* window = (SDL_Window*)CreateSDLWindow(INTEGRATIONDEMO_LAYOUT_WIDTH, INTEGRATIONDEMO_LAYOUT_HEIGHT);
	int sdlWidth, sdlHeight;
	SDL_GetWindowSizeInPixels(window, &sdlWidth, &sdlHeight);

	if ( !IntegrationDemo::Instance().Init(
		   window, &g_InputMgr, szError, (unsigned int)IntegrationDemoHelpers::AK_ARRAYSIZE(szError), sdlWidth, sdlHeight))
	{
		// Something went bad in the initialization
		// Post a modal alert and quit!
		NSAlert *alert = [[NSAlert alloc] init];
		if (alert != nullptr)
		{
			[alert setMessageText:@"Failed to initialize the Integration Demo"];
			[alert setInformativeText: [NSString stringWithCString:szError encoding:[NSString defaultCStringEncoding]]];
			[alert addButtonWithTitle:@"Ok"];

			[alert runModal];
			[alert release];
		}

		[NSApp terminate:self];

		return;
	}
	{
		// Save the NSWindow instance that acts as the SDL main window
		SDL_SysWMinfo wminfo;
		SDL_GetWindowWMInfo(window, &wminfo);
		m_window = (NSWindow*)wminfo.info.cocoa.window;
		if (!m_window)
		{
			NSAlert *alert = [[NSAlert alloc] init];
			if (alert != nullptr)
			{
				[alert setMessageText:@"Failed to initialize the Integration Demo"];
				[alert setInformativeText: @"The NSWindow instance was nil"];
				[alert addButtonWithTitle:@"Ok"];

				[alert runModal];
				[alert release];
			}

			[NSApp terminate:self];

			return;
		}
	}

	[self AddObservers];
	[NSApp setWindowsNeedUpdate:YES];
}

- (void)applicationWillUpdate:(NSNotification *)notification
{
	if (m_window)
	{
		// Records the starting time of the current frame
		IntegrationDemo::Instance().StartFrame();

		// Process the current frame, quit if Update() is false.
		if ( ! IntegrationDemo::Instance().Update() || g_InputMgr.ShouldQuit())
		{
			m_window = nil;
			[NSApp terminate:self];
			return; // Trigger a quit
		}
		IntegrationDemo::Instance().Render();

		// Ends the frame and regulates the application's framerate
		IntegrationDemo::Instance().EndFrame();
	}

	[NSApp setWindowsNeedUpdate:YES];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	// Unregister to notifications... otherwise applicationWillTerminate might be called twice
	[[NSNotificationCenter defaultCenter] removeObserver:self];

	// Terminate the integration demo (will terminate the wwise soundengine)
	IntegrationDemo::Instance().Term();
}

-(void) AddObservers
{    
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowWillClose:) name:NSWindowWillCloseNotification
											   object:m_window];

	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowsDidMiniaturize:) name:NSWindowDidMiniaturizeNotification
											   object:m_window];

	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowsDidDeminiaturize:) name:NSWindowDidDeminiaturizeNotification
											   object:m_window];
}

- (void)windowsDidMiniaturize:(NSNotification *)notification
{
	IntegrationDemo::Instance().Pause();
}

- (void)windowsDidDeminiaturize:(NSNotification *)notification
{
	IntegrationDemo::Instance().Resume();
}

- (void)windowWillClose:(NSNotification *)notification
{
	[[NSRunLoop currentRunLoop] performSelector:@selector(terminate:)
										 target:NSApp
									   argument:self
										  order:100
										  modes: [NSArray arrayWithObject:NSDefaultRunLoopMode]];
}

@end
