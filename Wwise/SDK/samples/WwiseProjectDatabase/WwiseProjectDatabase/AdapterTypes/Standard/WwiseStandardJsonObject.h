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

#include <fstream>

#include "rapidjson/allocators.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/istreamwrapper.h"

#include "AdapterTypes/WwiseJsonObject.h"
#include "AdapterTypes/Standard/WwiseStandardArray.h"
#include "AdapterTypes/Standard/WwiseStandardString.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "AdapterTypes/WwiseProjectDatabaseLogging.h"

class WwiseStandardJsonObject : public IWwiseJsonObject<rapidjson::GenericValue<rapidjson::UTF8<>>*>
{
	std::shared_ptr<rapidjson::Document> Document;
public:
	WwiseStandardJsonObject() : Document(std::make_shared<rapidjson::Document>()){}
	
	WwiseStandardJsonObject(const WwiseStandardString& FilePath)
	{
		Document = std::make_shared<rapidjson::Document>();
		std::filesystem::path path (reinterpret_cast<const char8_t*>(*FilePath));
		std::fstream stream = std::fstream(path);
		rapidjson::IStreamWrapper isw(stream);
		Document->ParseStream(isw);
		if(Document->HasParseError() || !Document->IsObject())
		{
			WWISE_DB_LOG(Error, "Error while decoding json");
		}
		else
		{
			JsonObject = Document.get();
		}
	}

	static bool CreateJsonObject(WwiseStandardString& File, WwiseStandardJsonObject& JsonObject)
	{
		JsonObject.Document->Parse(File.String.c_str());
		if(JsonObject.Document->GetParseError() != 0)
		{
			WWISE_DB_LOG(Error, "Error while decoding json");
			return false;
		}
		JsonObject.JsonObject = JsonObject.Document.get();	
		return true;
	}
	
	WwiseStandardJsonObject(rapidjson::GenericValue<rapidjson::UTF8<>>* Value)
	{
		JsonObject = Value;
	}

	~WwiseStandardJsonObject()
	{
		
	}

	bool TryGetBoolField(const WwiseStandardString& Name, bool& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(std::string(itr->name.GetString()) == Name.String && itr->value.IsString())
				{
					OutValue = itr->value.GetString() == std::string("true");
					if(OutValue)
					{
						return true;
					}
					if(itr->value.GetString() == std::string("false"))
					{
						OutValue = false;
						return true;
					}
					WWISE_DB_LOG(Error, "Error while decoding bool field %s", *Name);
					return false;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetBoolField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool TryGetIntField(const WwiseStandardString& Name, int& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(std::string(itr->name.GetString()) == Name.String && itr->value.IsString())
				{
					OutValue = std::atoi(itr->value.GetString());
					return true;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetIntField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool TryGetShortIdField(const WwiseStandardString& Name, WwiseDBShortId& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(std::string(itr->name.GetString()) == Name.String && itr->value.IsString())
				{
					OutValue = (unsigned int)std::atoll(itr->value.GetString());
					return true;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetShortIdField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool TryGetFloatField(const WwiseStandardString& Name, float& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(std::string(itr->name.GetString()) == Name.String && itr->value.IsString())
				{
					OutValue = (float)std::atof(itr->value.GetString());
					return true;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetFloatField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool TryGetDoubleField(const WwiseStandardString& Name, double& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(std::string(itr->name.GetString()) == Name.String && itr->value.IsString())
				{
					OutValue = std::atof(itr->value.GetString());
					return true;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetDoubleField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool TryGetObjectField(const WwiseStandardString& Name, WwiseStandardJsonObject& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(std::string(itr->name.GetString()) == Name.String && itr->value.IsObject())
				{
					auto& Value = JsonObject->operator[](*Name);
					OutValue = WwiseStandardJsonObject(&Value);
					return true;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetObjectField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	
	bool TryGetStringField(const WwiseStandardString& Name, WwiseStandardString& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(itr->name.GetString() == Name.String && itr->value.IsString())
				{
					OutValue = WwiseStandardString(itr->value.GetString());
					return true;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetStringField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool TryGetArrayField(const WwiseStandardString& Name, WwiseStandardArray<WwiseStandardJsonObject>& OutValue) const
	{
		if(JsonObject->IsObject())
		{
			for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
			{
				if(std::string(itr->name.GetString()) == Name.String && itr->value.IsArray())
				{
					auto Array = itr->value.GetArray();
					for(auto& Elem : Array)
					{
						OutValue.Add(WwiseStandardJsonObject(&Elem));
					}
					return true;
				}
				if(itr->value.IsObject())
				{
					WwiseStandardJsonObject Object(&itr->value);
					if(Object.TryGetArrayField(Name, OutValue))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	int GetKeys(WwiseStandardArray<WwiseStandardString>& OutKeys) const
	{
		for(auto itr = JsonObject->MemberBegin(); itr != JsonObject->MemberEnd(); ++itr)
		{
			auto name = itr->name.GetString();
			OutKeys.Add(WwiseStandardString(name));
		}
		return (int)OutKeys.Size();	
	}
};
