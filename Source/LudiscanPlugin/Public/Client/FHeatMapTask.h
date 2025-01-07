#pragma once
#include "FHeatmapData.h"
#include "FPlaySession.h"
#include "Project.h"


struct FHeatMapTask
{
	enum ETaskStatus
	{
		Pending,
		Processing,
		Completed,
		Failed
	};
	int TaskId;

	FProject Project;

	FPlaySession Session;

	int StepSize;

	bool ZVisible;

	ETaskStatus Status;

	TArray<FHeatmapData> HeatMapDataArray;

	FString CreateAt;

	FString UpdateAt;

	FHeatMapTask()
		: TaskId(0)
		  , Project(FProject())
		  , StepSize(0)
		  , ZVisible(false)
		  , Status(ETaskStatus::Pending), HeatMapDataArray()
	{
	}

	static bool ParseDataFromJson(const FString& JsonString, FHeatMapTask& OutData)
	{
		if (JsonString.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Empty JsonString"));
			return false;
		}
		UE_LOG(LogTemp, Log, TEXT("JsonString: %s"), *JsonString);
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		if (!FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize JSON"));
			return false;
		}
		if (!JsonObject.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid JsonObject"));
			return false;
		}

		OutData.TaskId = JsonObject->GetIntegerField(TEXT("taskId"));
		FProject Project;
		TSharedPtr<FJsonObject> ProjectJsonObject = JsonObject->GetObjectField(TEXT("project"));
		if (FProject::ParseObjectFromJson(ProjectJsonObject.ToSharedRef(), Project))
		{
			OutData.Project = Project;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to convert JsonObject to FProject"));
			return false;
		}
		FPlaySession Session;
		const TSharedPtr<FJsonObject>* SessionString;
		if (JsonObject->TryGetObjectField(TEXT("session"), SessionString))
		{
			if (FPlaySession::ParseDataFromJson(SessionString->ToSharedRef(), Session))
			{
				OutData.Session = Session;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No session data"));
			}
		}
		JsonObject->TryGetNumberField(TEXT("stepSize"), OutData.StepSize);
		JsonObject->TryGetBoolField(TEXT("zVisible"), OutData.ZVisible);
		FString StatusString;
		if (JsonObject->TryGetStringField(TEXT("status"), StatusString))
		{
			if (StatusString == TEXT("pending"))
			{
				OutData.Status = ETaskStatus::Pending;
			}
			else if (StatusString == TEXT("processing"))
			{
				OutData.Status = ETaskStatus::Processing;
			}
			else if (StatusString == TEXT("completed"))
			{
				OutData.Status = ETaskStatus::Completed;
			}
			else if (StatusString == TEXT("failed"))
			{
				OutData.Status = ETaskStatus::Failed;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid status string"));
				return false;
			}
		}
		const TArray<TSharedPtr<FJsonValue>>* HeatmapArray = nullptr;
		if (JsonObject->TryGetArrayField(TEXT("result"), HeatmapArray))
		{
			if (FHeatmapData::ParseArrayFromJson(HeatmapArray, OutData.HeatMapDataArray))
			{
				UE_LOG(LogTemp, Log, TEXT("Parsed HeatmapDataArray: size=%d"), OutData.HeatMapDataArray.Num());
			} else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to parse HeatmapDataArray"));
				return false;
			}
		} else
		{
			OutData.HeatMapDataArray = TArray<FHeatmapData>();
		}
		JsonObject->TryGetStringField(TEXT("createdAt"), OutData.CreateAt);
		JsonObject->TryGetStringField(TEXT("updatedAt"), OutData.UpdateAt);
		return true;
	}

	void Log()
	{
		UE_LOG(LogTemp, Log, TEXT("TaskId: %d"), TaskId);
		UE_LOG(LogTemp, Log, TEXT("Project:"));
		Project.Log();
		UE_LOG(LogTemp, Log, TEXT("Session:"));
		Session.Log();
		UE_LOG(LogTemp, Log, TEXT("StepSize: %d"), StepSize);
		UE_LOG(LogTemp, Log, TEXT("ZVisible: %s"), ZVisible ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Log, TEXT("Status: %s"), Status == ETaskStatus::Pending ? TEXT("Pending") : Status == ETaskStatus::Processing ? TEXT("Processing") : Status == ETaskStatus::Completed ? TEXT("Completed") : TEXT("Failed"));
		UE_LOG(LogTemp, Log, TEXT("HeatMapData: size=%d"), HeatMapDataArray.Num());
		UE_LOG(LogTemp, Log, TEXT("CreateAt: %s"), *CreateAt);
		UE_LOG(LogTemp, Log, TEXT("UpdateAt: %s"), *UpdateAt);
	}

};
