#pragma once

#include "JsonObjectConverter.h"

struct FProject
{
	int Id;

	FString Name;

	FString Description;

	FString CreatedAt;

	FProject(): Id(0), Name(""), Description(""), CreatedAt("")
	{
	}

	static bool ParseObjectFromJson(const TSharedPtr<FJsonObject>& JsonObject, FProject& OutData)
	{
		if (JsonObject.IsValid())
		{
			OutData.Id = JsonObject->GetIntegerField(TEXT("id"));
			OutData.Name = JsonObject->GetStringField(TEXT("name"));
			OutData.Description = JsonObject->GetStringField(TEXT("description"));
			OutData.CreatedAt = JsonObject->GetStringField(TEXT("createdAt"));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid JsonObject"));
			return false;
		}
	}

	static bool ParseArrayFromJson(const FString& JsonString, TArray<FProject>& OutArray)
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(Reader, JsonArray))
		{
			for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
			{
				TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
				if (JsonObject.IsValid())
				{
					FProject Item;
					if (ParseObjectFromJson(JsonObject.ToSharedRef(), Item))
					{
						OutArray.Add(Item);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON object"));
						return false;
					}
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
		UE_LOG(LogTemp, Log, TEXT("Id: %d"), Id);
		UE_LOG(LogTemp, Log, TEXT("Name: %s"), *Name);
		UE_LOG(LogTemp, Log, TEXT("Description: %s"), *Description);
		UE_LOG(LogTemp, Log, TEXT("CreatedAt: %s"), *CreatedAt);
	}
};
