#pragma once

struct FPlaySession
{

	int32 SessionId;

	int32 ProjectId;

	FString Name;

	FString DeviceId;

	FString Platform;

	FString AppVersion;

	TMap<FString, FString> MetaData;

	FString StartTime;

	FString EndTime;

	bool bIsPlaying;
	// JSONの自動変換を可能にするために必要
	FPlaySession()
		: SessionId(0), ProjectId(0), bIsPlaying(false)
	{}

	// JSON オブジェクトから単一のデータをパースするメソッド
	static bool ParseDataFromJson(const TSharedPtr<FJsonObject>& JsonObject, FPlaySession& OutData)
	{
		if (!JsonObject.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid JsonObject"));
			return false;
		}

		// 各フィールドをパース
		OutData.SessionId = JsonObject->GetIntegerField(TEXT("sessionId"));
		OutData.ProjectId = JsonObject->GetIntegerField(TEXT("projectId"));
		OutData.Name = JsonObject->GetStringField(TEXT("name"));
		OutData.DeviceId = JsonObject->GetStringField(TEXT("deviceId"));
		OutData.Platform = JsonObject->GetStringField(TEXT("platform"));
		OutData.AppVersion = JsonObject->GetStringField(TEXT("appVersion"));

		// MetaData を TMap<FString, FString> に変換
		const TSharedPtr<FJsonObject>* MetaDataObject;
		if (JsonObject->TryGetObjectField(TEXT("metaData"), MetaDataObject))
		{
			if (MetaDataObject->IsValid())
			{
				for (const auto& Pair : MetaDataObject->Get()->Values)
				{
					OutData.MetaData.Add(Pair.Key, Pair.Value->AsString());
				}
			}
		}
		OutData.StartTime = JsonObject->GetStringField(TEXT("startTime"));
		OutData.EndTime = JsonObject->GetStringField(TEXT("endTime"));
		OutData.bIsPlaying = JsonObject->GetBoolField(TEXT("isPlaying"));

		return true;
	}

	// JSON 配列から複数のデータをパースするメソッド
	static bool ParseArrayFromJson(const FString& JsonString, TArray<FPlaySession>& OutArray)
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(Reader, JsonArray))
		{
			for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
			{
				FPlaySession Data;
				if (ParseDataFromJson(JsonValue->AsObject(), Data))
				{
					OutArray.Add(Data);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON object"));
					return false;
				}
			}
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON array"));
			return false;
		}
	}

	void Log()
	{
		UE_LOG(LogTemp, Log, TEXT("SessionId: %d"), SessionId);
		UE_LOG(LogTemp, Log, TEXT("ProjectId: %d"), ProjectId);
		UE_LOG(LogTemp, Log, TEXT("Name: %s"), *Name);
		UE_LOG(LogTemp, Log, TEXT("DeviceId: %s"), *DeviceId);
		UE_LOG(LogTemp, Log, TEXT("Platform: %s"), *Platform);
		UE_LOG(LogTemp, Log, TEXT("AppVersion: %s"), *AppVersion);
		UE_LOG(LogTemp, Log, TEXT("MetaData:"));
		for (const auto& Pair : MetaData)
		{
			UE_LOG(LogTemp, Log, TEXT("Key: %s, Value: %s"), *Pair.Key, *Pair.Value);
		}
		UE_LOG(LogTemp, Log, TEXT("StartTime: %s"), *StartTime);
		UE_LOG(LogTemp, Log, TEXT("EndTime: %s"), *EndTime);
		UE_LOG(LogTemp, Log, TEXT("bIsPlaying: %d"), bIsPlaying);
	}
};
