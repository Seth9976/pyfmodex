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

#include "Metadata/WwiseMetadataLoadable.h"

struct WwiseMetadataSettings : public WwiseMetadataLoadable
{
	bool bAutoSoundBankDefinition;
	bool bCopyLooseStreamedMediaFiles;
	bool bSubFoldersForGeneratedFiles;
	bool bRemoveUnusedGeneratedFiles;
	bool bSourceControlGeneratedFiles;
	bool bGenerateHeaderFile;
	bool bGenerateContentTxtFile;
	bool bGenerateMetadataXML;
	bool bGenerateMetadataJSON;
	bool bGenerateAllBanksMetadata;
	bool bGeneratePerBankMetadata;
	bool bUseSoundBankNames;
	bool bAllowExceedingMaxSize;
	bool bMaxAttenuationInfo;
	bool bEstimatedDurationInfo;
	bool bPrintObjectGuid;
	bool bPrintObjectPath;
	bool bPrintObjectColor;

	WwiseMetadataSettings();
	WwiseMetadataSettings(WwiseMetadataLoader& Loader);
};
