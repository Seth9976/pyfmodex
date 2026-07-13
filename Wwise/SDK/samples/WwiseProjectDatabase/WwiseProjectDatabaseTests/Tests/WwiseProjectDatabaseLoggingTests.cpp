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

#include <catch2/catch_test_macros.hpp>

#include "WwiseProjectDatabase.h"
#include "AdapterTypes/WwiseWrapperTypes.h"

SCENARIO("Wwise::WwiseProjectDatabase::Logging")
{
    SECTION("Expected Error")
    {
        WwiseProjectDatabaseLoggingUtils::ResetErrors();
        WwiseProjectDatabaseLoggingUtils::AddExpectedError("An Expected Error");
        WWISE_DB_LOG(Error, "An Expected Error");
        WwiseProjectDatabaseLoggingUtils::ParseErrors();
    }
    
    SECTION("Expected Error with Multiple Occurences")
    {
        WwiseProjectDatabaseLoggingUtils::ResetErrors();
        WwiseProjectDatabaseLoggingUtils::AddExpectedError("Multiple Errors", 2);
        WWISE_DB_LOG(Error, "Multiple Errors");
        WWISE_DB_LOG(Error, "Multiple Errors");
        WwiseProjectDatabaseLoggingUtils::ParseErrors();
    }
}
