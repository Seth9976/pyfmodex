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

#include "${name}Sink.h"
#include "../${name}Config.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* Create${name}Sink(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, ${name}Sink());
}

AK::IAkPluginParam* Create${name}SinkParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, ${name}SinkParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(${name}Sink, AkPluginTypeSink, ${name}Config::CompanyID, ${name}Config::PluginID)

${name}Sink::${name}Sink()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
    , m_bStarved(false)
    , m_bDataReady(false)
{
}

${name}Sink::~${name}Sink()
{
}

AKRESULT ${name}Sink::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSinkPluginContext* in_pCtx, AK::IAkPluginParam* in_pParams, AkAudioFormat& io_rFormat)
{
    m_pParams = (${name}SinkParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pCtx;

    // Stereo config taking 32-bit float interleaved at 48kHz
    AkChannelConfig channelConfig;
    channelConfig.SetStandard(AK_SPEAKER_SETUP_STEREO);
    io_rFormat.SetAll(
        48'000,
        channelConfig,
        32,
        channelConfig.uNumChannels * (32 / 8),
        AK_FLOAT,
        AK_INTERLEAVED
    );

    return AK_Success;
}

AKRESULT ${name}Sink::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT ${name}Sink::Reset()
{
    return AK_Success;
}

AKRESULT ${name}Sink::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeSink;
    out_rPluginInfo.bIsInPlace = true;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

AKRESULT ${name}Sink::IsDataNeeded(AkUInt32& out_uNumFramesNeeded)
{
    // Set the number of frames needed here
    out_uNumFramesNeeded = m_pContext->GetNumRefillsInVoice();
    return AK_Success;
}

void ${name}Sink::Consume(AkAudioBuffer* in_pInputBuffer, AkRamp in_gain)
{
    if (in_pInputBuffer->uValidFrames > 0)
    {
        // Consume input buffer and send it to the output here
        m_bDataReady = true;
    }
}

void ${name}Sink::OnFrameEnd()
{
    if (!m_bDataReady)
    {
        // Consume was not called for this audio frame, send silence to the output here
    }

    m_bDataReady = false;
}

bool ${name}Sink::IsStarved()
{
    return m_bStarved;
}

void ${name}Sink::ResetStarved()
{
    m_bStarved = false;
}
