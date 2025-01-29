#pragma once

#include "OpenAPIProjectResponseDto.h"

struct FProject
{
	int Id;

	FString Name;

	FString Description;

	FDateTime CreatedAt;

	FProject(): Id(0), Name(""), Description(""), CreatedAt(FDateTime())
	{
	}

	static FProject ParseFromOpenAPIProjectResponseDto(const OpenAPI::OpenAPIProjectResponseDto& Response)
	{
		FProject Project;
		Project.Id = Response.Id;
		Project.Name = Response.Name;
		Project.Description = Response.Description;
		Project.CreatedAt = Response.CreatedAt;
		return Project;
	}

	void Log()
	{
		UE_LOG(LogTemp, Log, TEXT("Id: %d"), Id);
		UE_LOG(LogTemp, Log, TEXT("Name: %s"), *Name);
		UE_LOG(LogTemp, Log, TEXT("Description: %s"), *Description);
		UE_LOG(LogTemp, Log, TEXT("CreatedAt: %s"), *CreatedAt.ToString());
	}
};
