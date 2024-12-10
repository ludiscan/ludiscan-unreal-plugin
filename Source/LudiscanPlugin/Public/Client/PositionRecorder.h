// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LudiscanClient.h"
#include "UObject/Object.h"
#include "PositionRecorder.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, BlueprintType)
class LUDISCANPLUGIN_API UPositionRecorder : public UObject
{
	GENERATED_BODY()
public:
	UPositionRecorder();

	void CreateSession(
		UWorld* Context,
		int NewProjectId,
		FString SessionTitle,
		TFunction<void(FPlaySession)> OnResponse
		);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"), Category = "Ludiscan|Session")
	void StartSession(
	UObject* WorldContextObject)
	{
		const int ProjectId = LudiscanClient::GetSaveProjectId(1);
		CreateSession(
		WorldContextObject->GetWorld(),
			ProjectId,
			"TestSession",
			[this](FPlaySession PlaySession) {
				UE_LOG(LogTemp, Log, TEXT("Session ID: %d"), PlaySession.SessionId);
				StartRecording(GetWorld());
			}
		);
	}

	UFUNCTION(BlueprintCallable, Category = "Ludiscan|Session")
	void StartRecording(UWorld* Context);

	UFUNCTION(BlueprintCallable, Category = "Ludiscan|Session")
	void StopRecording();

	UFUNCTION(BlueprintCallable, Category = "Ludiscan|Session")
	void FinishedSession();

	void UpdateSessionData(
		TMap<FString, FString> ExtraData,
		TFunction<void()> OnSuccess = []() {}
	);

	const TArray<TArray<FPlayerPosition>>& GetPositionData() const;

	UFUNCTION(BlueprintCallable, Category = "Ludiscan|Session")
	void AddMetaData(
		TMap<FString, FString> ExtraData
		)
	{
		UpdateSessionData(ExtraData);
	}

private:
	TArray<TArray<FPlayerPosition>> PositionData;
	FTimerHandle TimerHandle;
	uint64 StartTime;

	UPROPERTY()
	UWorld* WorldContext;

	LudiscanClient Client;

	// 実行中のsession
	FPlaySession PlaySessionCreate;

	void RecordPlayerPositions();
};
