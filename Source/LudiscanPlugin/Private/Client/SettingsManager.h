#pragma once

class FSettingsManager
{
public:
	static void SetFloat(const FString& Key, float Value)
	{
		SaveSetting(Key, FString::SanitizeFloat(Value));
	}
	static void SetInt(const FString& Key, int Value)
	{
		SaveSetting(Key, FString::FromInt(Value));
	}
	static void SetString(const FString& Key, const FString& Value)
	{
		SaveSetting(Key, Value);
	}
	static float GetFloat(const FString& Key, float DefaultValue)
	{
		const FString Value = LoadSetting(Key);
		return Value.IsEmpty() ? DefaultValue : FCString::Atof(*Value);
	}
	static int GetInt(const FString& Key, int DefaultValue)
	{
		const FString Value = LoadSetting(Key);
		return Value.IsEmpty() ? DefaultValue : FCString::Atoi(*Value);
	}
	static FString GetString(const FString& Key, const FString& DefaultValue)
	{
		const FString Value = LoadSetting(Key);
		return Value.IsEmpty() ? DefaultValue : Value;
	}
private:
	static void SaveSetting(const FString& Key, const FString& Value)
	{
		GConfig->SetString(TEXT("/Script/Engine.GameSettings"), *Key, *Value, GGameIni);
	}

	static FString LoadSetting(const FString& Key)
	{
		FString Value;
		GConfig->GetString(TEXT("/Script/Engine.GameSettings"), *Key, Value, GGameIni);
		return Value;
	}
};