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

#include "Metadata/WwiseMetadataProject.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataProject::WwiseMetadataProject()
{
	WWISE_DB_LOG(Error, "Using default Project");
}

WwiseMetadataProject::WwiseMetadataProject(WwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, "Name"_wwise_db)),
	GUID(Loader.GetGuid(this, "GUID"_wwise_db)),
	Generator(Loader.GetString(this, "Generator"_wwise_db))
{
	Loader.LogParsed("Project"_wwise_db);
}
