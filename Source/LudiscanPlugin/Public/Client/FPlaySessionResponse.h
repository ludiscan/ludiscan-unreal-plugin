#pragma once

#include "CoreMinimal.h"
#include "FPlaySessionResponse.generated.h"

USTRUCT(BlueprintType)
struct FPlaySessionResponseDto
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SessionId;

	UPROPERTY()
	int32 ProjectId;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString DeviceId;

	UPROPERTY()
	FString Platform;

	UPROPERTY()
	FString AppVersion;

	UPROPERTY()
	TMap<FString, FString> MetaData;

	UPROPERTY()
	FString StartTime;

	UPROPERTY()
	FString EndTime;

	UPROPERTY()
	bool bIsPlaying;
	// JSONの自動変換を可能にするために必要
	FPlaySessionResponseDto()
		: SessionId(0), ProjectId(0), bIsPlaying(false)
	{}

	// JSON オブジェクトから単一のデータをパースするメソッド
	static bool ParseDataFromJson(const TSharedPtr<FJsonObject>& JsonObject, FPlaySessionResponseDto& OutData)
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
		TSharedPtr<FJsonObject> MetaDataObject = JsonObject->GetObjectField(TEXT("metaData"));
		if (MetaDataObject.IsValid())
		{
			for (const auto& Pair : MetaDataObject->Values)
			{
				OutData.MetaData.Add(Pair.Key, Pair.Value->AsString());
			}
		}

		OutData.StartTime = JsonObject->GetStringField(TEXT("startTime"));
		OutData.EndTime = JsonObject->GetStringField(TEXT("endTime"));
		OutData.bIsPlaying = JsonObject->GetBoolField(TEXT("isPlaying"));

		return true;
	}

	// JSON 配列から複数のデータをパースするメソッド
	static bool ParseArrayFromJson(const FString& JsonString, TArray<FPlaySessionResponseDto>& OutArray)
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(Reader, JsonArray))
		{
			for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
			{
				FPlaySessionResponseDto Data;
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
