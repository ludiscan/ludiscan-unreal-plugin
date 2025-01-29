#pragma once
#include "OpenAPIPlaySessionResponseDto.h"

struct LUDISCANPLUGIN_API FPlaySession
{

	int32 SessionId;

	int32 ProjectId;

	FString Name;

	FString DeviceId;

	FString Platform;

	FString AppVersion;

	TMap<FString, FString> MetaData;

	FDateTime StartTime;

	FDateTime EndTime;

	bool IsPlaying;
	// JSONの自動変換を可能にするために必要
	FPlaySession()
		: SessionId(0), ProjectId(0), IsPlaying(false)
	{}

	static FPlaySession ParseFromOpenAPIPlaySessionResponseDto(const OpenAPI::OpenAPIPlaySessionResponseDto& Response)
	{
		FPlaySession PlaySession;
		PlaySession.SessionId = Response.SessionId;
		PlaySession.ProjectId = Response.ProjectId;
		PlaySession.Name = Response.Name;
		PlaySession.DeviceId = Response.DeviceId ? Response.DeviceId.GetValue() : "";
		PlaySession.Platform = Response.Platform ? Response.Platform.GetValue() : "";
		PlaySession.AppVersion = Response.AppVersion ? Response.AppVersion.GetValue() : "";
		TSharedPtr<FJsonObject> MetaData = Response.MetaData ? Response.MetaData.GetValue() : nullptr;
		if (MetaData.IsValid())
		{
			PlaySession.MetaData = TMap<FString, FString>();
			for (const auto& Pair : MetaData->Values)
			{
				PlaySession.MetaData.Add(Pair.Key, Pair.Value->AsString());
			}
		}
		PlaySession.StartTime = Response.StartTime;
		PlaySession.EndTime = Response.EndTime ? Response.EndTime.GetValue() : FDateTime();
		PlaySession.IsPlaying = Response.IsPlaying;
		return PlaySession;
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
		UE_LOG(LogTemp, Log, TEXT("StartTime: %s"), *StartTime.ToString());
		UE_LOG(LogTemp, Log, TEXT("EndTime: %s"), *EndTime.ToString());
		UE_LOG(LogTemp, Log, TEXT("bIsPlaying: %d"), IsPlaying);
	}
};
