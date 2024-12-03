// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LudiscanClient.h"
#include "UObject/Object.h"
#include "PositionRecorder.generated.h"

/**
 * 
 */
UCLASS()
class LUDISCANPLUGIN_API UPositionRecorder : public UObject
{
	GENERATED_BODY()
public:
	UPositionRecorder();

	void CreateSession(
		UWorld* Context,
		int NewProjectId,
		FString SessionTitle = "TitleNone",
		TFunction<void(FPlaySessionCreate)> OnResponse = [](FPlaySessionCreate PlaySession) {}
		);

	void StartRecording(UWorld* Context);
	void StopRecording();

	void FinishedRecording();
	const TArray<TArray<FPlayerPosition>>& GetPositionData() const;

private:
	TArray<TArray<FPlayerPosition>> PositionData;
	FTimerHandle TimerHandle;
	uint64 StartTime;

	UPROPERTY()
	UWorld* WorldContext;

	LudiscanClient Client;

	// 実行中のsession
	FPlaySessionCreate PlaySessionCreate;

	void RecordPlayerPositions();
};
