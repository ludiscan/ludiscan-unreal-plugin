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
		const int ProjectId = LudiscanAPI::LudiscanClient::GetSaveProjectId(1);
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

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"), Category = "Ludiscan|Session")
	void StartSessionWithSetProject(
		UObject* WorldContextObject,
		int ProjectId)
	{
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
	void UploadPositions();

	UFUNCTION(BlueprintCallable, Category = "Ludiscan|Session")
	void FinishedSession();

	void UpdateSessionData(
		TMap<FString, FString> ExtraData,
		TFunction<void()> OnSuccess = []() {}
	);

	const TArray<TArray<LudiscanAPI::FPlayerPosition>>& GetPositionData() const;

	UFUNCTION(BlueprintCallable, Category = "Ludiscan|Session")
	void AddMetaData(
		TMap<FString, FString> ExtraData
		)
	{
		UpdateSessionData(ExtraData);
	}

private:
	TArray<TArray<LudiscanAPI::FPlayerPosition>> PositionData;
	FTimerHandle TimerHandle;
	uint64 StartTime;

	UPROPERTY()
	UWorld* WorldContext;

	LudiscanAPI::LudiscanClient Client = LudiscanAPI::LudiscanClient();

	// 実行中のsession
	FPlaySession PlaySessionCreate;

	UPROPERTY(BlueprintReadOnly, Category = "Ludiscan|Session", meta = (AllowPrivateAccess = "true"))
	bool IsInSession = false;

	void RecordPlayerPositions();
};
