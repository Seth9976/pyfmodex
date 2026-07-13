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

#ifdef EXPORTING_WWISE_PROJECT_DATABASE
#ifdef _WIN32
   #define WWISE_PROJECT_DATABASE_API __declspec(dllexport)
#else
    #define WWISE_PROJECT_DATABASE_API __attribute__((visibility("default")))
#endif
#elif defined(TESTING_WWISE_PROJECT_DATABASE)
    #define WWISE_PROJECT_DATABASE_API
#else
#ifdef _WIN32
   #define WWISE_PROJECT_DATABASE_API __declspec(dllimport)
#else
    #define WWISE_PROJECT_DATABASE_API __attribute__((visibility("default")))
#endif
#endif