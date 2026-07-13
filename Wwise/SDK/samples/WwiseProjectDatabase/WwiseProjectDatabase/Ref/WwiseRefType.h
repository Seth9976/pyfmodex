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

enum class WwiseRefType : WwiseDBShortId
{
	RootFile,
		
		ProjectInfo,
			Language,

		PlatformInfo,
			Platform,
		
		PluginInfo,
			PluginLib,

		SoundBanksInfo,
			SoundBank,
				Media,
				CustomPlugin,
				PluginShareSet,
				AudioDevice,
				Event,
					SwitchContainer,
				DialogueEvent,
					DialogueArgument,
				Bus,
				AuxBus,
				GameParameter,
				StateGroup,
					State,
				SwitchGroup,
					Switch,
				Trigger,
				ExternalSource,
				AcousticTexture,

	None = (WwiseDBShortId)-1
};

using WwiseRefIndexType = unsigned int;
