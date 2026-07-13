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

#if defined(__has_include) && !defined(INTDEMO_GME) && defined(AK_PLATFORM_SUPPORTS_GME)
// Enable GME demo if Tencent GME plugin is installed locally and if it is supported on the target platform
#if __has_include(<AK/Plugin/TencentGMEPlugin.h>)

#if defined(AK_PS4) || defined(AK_PS5) || defined(AK_IOS) || defined(AK_NX)
  #define INTDEMO_GME
  #pragma comment(lib, "GMESDK")
#else // Windows, Xbox, ... other supported platforms that does not require static linking with GMESDK
  #define INTDEMO_GME
#endif

#if defined(INTDEMO_GME)
#include <AK/Plugin/TencentGMEPlugin.h>
#pragma comment(lib, "TencentGMEPlugin")

#if defined(AK_IOS)
#pragma comment(lib, "iconv")
#pragma comment(lib, "resolv")
#endif

#endif // INTDEMO_GME

#endif // __has_include

#endif // defined(__has_include)