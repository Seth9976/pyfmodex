/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#include "${name}Meta.h"
#include "../${name}Config.h"
#include <AK/Tools/Common/AkBankReadHelpers.h>

AK::IAkPluginParam * Create${name}Meta( AK::IAkPluginMemAlloc * in_pAllocator )
{
	return AK_PLUGIN_NEW( in_pAllocator, ${name}Meta() );
}

AK::PluginRegistration ${name}MetaRegistration(AkPluginTypeMetadata, ${name}Config::CompanyID, ${name}Config::PluginID, NULL, Create${name}Meta);

${name}Meta::${name}Meta( )
{
}

${name}Meta::~${name}Meta( )
{
}

${name}Meta::${name}Meta( const ${name}Meta& in_rParams )
{
	RTPC = in_rParams.RTPC;
	NonRTPC = in_rParams.NonRTPC;
	m_paramChangeHandler.SetAllParamChanges();
}

AK::IAkPluginParam * ${name}Meta::Clone( AK::IAkPluginMemAlloc * in_pAllocator )
{
	AKASSERT( in_pAllocator != NULL );
	return AK_PLUGIN_NEW( in_pAllocator, ${name}Meta( *this ) );
}

AKRESULT ${name}Meta::Init(	AK::IAkPluginMemAlloc *	in_pAllocator,
							const void *			in_pParamsBlock, 
							AkUInt32				in_ulBlockSize )
{
	if ( in_ulBlockSize == 0)
	{
		// Initialize default parameters here
		RTPC.fPlaceholder = 0.0f;
		m_paramChangeHandler.SetAllParamChanges();
		return AK_Success;
	}
	return SetParamsBlock( in_pParamsBlock, in_ulBlockSize );
}

AKRESULT ${name}Meta::Term( AK::IAkPluginMemAlloc * in_pAllocator )
{
	AKASSERT( in_pAllocator != NULL );
	AK_PLUGIN_DELETE( in_pAllocator, this );
	return AK_Success;
}

// Blob set.
AKRESULT ${name}Meta::SetParamsBlock(	const void * in_pParamsBlock,
											AkUInt32 in_ulBlockSize
											)
{  
	AKRESULT eResult = AK_Success;
	AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

	// Read bank data here
	RTPC.fPlaceholder = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
	CHECKBANKDATASIZE(in_ulBlockSize, eResult);
	m_paramChangeHandler.SetAllParamChanges();

	return eResult;
}

// Update one parameter.
AKRESULT ${name}Meta::SetParam(	AkPluginParamID in_ParamID,
								const void * in_pValue, 
								AkUInt32 in_ulParamSize )
{
	AKASSERT( in_pValue != NULL );
	if ( in_pValue == NULL )
	{
		return AK_InvalidParameter;
	}
	AKRESULT eResult = AK_Success;

    // Handle parameter change here
	switch ( in_ParamID )
	{
	case PARAM_PLACEHOLDER_ID:
		RTPC.fPlaceholder = *((AkReal32*)in_pValue);
		m_paramChangeHandler.SetParamChange(PARAM_PLACEHOLDER_ID);
		break;
	default:
		AKASSERT(!"Invalid parameter.");
		eResult = AK_InvalidParameter;
		break;
	}

	return eResult;
}