#pragma once
#include "FHeatmapData.h"
#include "FPlaySession.h"
#include "OpenAPIHeatmapTaskDto.h"
#include "Project.h"

using OpenAPI::OpenAPIHeatmapTaskDto;

struct LUDISCANPLUGIN_API FHeatMapTask
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

	FDateTime CreateAt;

	FDateTime UpdateAt;

	FHeatMapTask()
		: TaskId(0)
		  , Project(FProject())
		  , StepSize(0)
		  , ZVisible(false)
		  , Status(Pending), HeatMapDataArray()
	{
	}

	static FHeatMapTask ParseFromOpenAPIHeatmapTaskDto(const OpenAPIHeatmapTaskDto& Dto)
	{
		FHeatMapTask Task;
		Task.TaskId = Dto.TaskId;
		Task.Project = FProject::ParseFromOpenAPIProjectResponseDto(Dto.Project);
		Task.Session = Dto.Session ? FPlaySession::ParseFromOpenAPIPlaySessionResponseDto(Dto.Session.GetValue()) : FPlaySession();
		Task.StepSize = Dto.StepSize;
		Task.ZVisible = Dto.ZVisible;
		Task.HeatMapDataArray = TArray<FHeatmapData>();
		for (const OpenAPI::OpenAPIHeatMapTaskResultListItem& Item : Dto.Result.GetValue())
		{
			Task.HeatMapDataArray.Add(FHeatmapData::ParseFromOpenAPIHeatMapTaskResultListItem(Item));
		}
		Task.Status = ParseFromStatusEnum(Dto.Status);
		Task.CreateAt = Dto.CreatedAt;
		Task.UpdateAt = Dto.UpdatedAt;
		return Task;
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
		UE_LOG(LogTemp, Log, TEXT("CreateAt: %s"), *CreateAt.ToString());
		UE_LOG(LogTemp, Log, TEXT("UpdateAt: %s"), *UpdateAt.ToString());
	}
private:
	static ETaskStatus ParseFromStatusEnum(const OpenAPIHeatmapTaskDto::StatusEnum& Status)
	{
		if (Status == OpenAPIHeatmapTaskDto::StatusEnum::Pending)
		{
			return Pending;
		}
		if (Status == OpenAPIHeatmapTaskDto::StatusEnum::Processing)
		{
			return Processing;
		}
		if (Status == OpenAPIHeatmapTaskDto::StatusEnum::Completed)
		{
			return Completed;
		}
		return Failed;
	}
};
