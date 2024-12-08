#pragma once

struct FHeatmapData
{
	
	float X;

	float Y;

	float Z;

	float Density;

	FHeatmapData(): X(0), Y(0), Z(0), Density(0)
	{
	}

	static bool ParseArrayFromJson(const TArray<TSharedPtr<FJsonValue>>* JsonString, TArray<FHeatmapData>& OutArray)
	{
		if (JsonString == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid JsonString"));
			return false;
		}

		for (const TSharedPtr<FJsonValue>& JsonValue : *JsonString)
		{
			TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
			if (JsonObject.IsValid())
			{
				FHeatmapData Item;
				if (JsonObject->TryGetNumberField(TEXT("x"), Item.X) &&
					JsonObject->TryGetNumberField(TEXT("y"), Item.Y) &&
					JsonObject->TryGetNumberField(TEXT("z"), Item.Z) &&
					JsonObject->TryGetNumberField(TEXT("density"), Item.Density))
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
};
