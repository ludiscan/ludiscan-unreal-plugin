#pragma once
#include "JsonObjectConverter.h"

#include "Project.generated.h"


USTRUCT(BlueprintType)
struct FProject
{
	GENERATED_BODY()

	UPROPERTY()
	int Id;
	
	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Description;

	UPROPERTY()
	FString CreatedAt;

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
					if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &Item))
					{
						OutArray.Add(Item);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to convert JsonObject to FPlaySessionResponseDto"));
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
};
