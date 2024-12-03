// Fill out your copyright notice in the Description page of Project Settings.


#include "Client/PositionRecorder.h"

#include "Client/LudiscanClient.h"

UPositionRecorder::UPositionRecorder(): StartTime(0), WorldContext(nullptr)
{
	Client = LudiscanClient();
	FString HostName = LudiscanClient::GetSaveApiHostName("https://yuhi.tokyo");
	Client.SetConfig(HostName);
}

void UPositionRecorder::CreateSession(
	UWorld* Context,
	int NewProjectId,
	FString SessionTItle,
	TFunction<void(FPlaySessionCreate)> OnResponse
)
{
	FString DeviceId = FPlatformProcess::ComputerName();
	FString platform = FPlatformProperties::IniPlatformName();
	FString appVersion = FApp::GetBuildVersion();
	//
	FString LevelName = Context->GetMapName();
	TMap<FString, FString> extraData = TMap<FString, FString>();
	extraData.Add("hello", "world");
	extraData.Add("foo", "bar");
	extraData.Add("baz", "qux");
	Client.CreateSession(
		NewProjectId,
		SessionTItle,
		DeviceId,
		platform,
		appVersion,
		LevelName,
		extraData,
		[this, OnResponse](FPlaySessionCreate PlaySession) {
			PlaySessionCreate = PlaySession;
			OnResponse(PlaySession);
		}
	);
}

void UPositionRecorder::StartRecording(UWorld* Context) {
	this->WorldContext = Context;
	if (TimerHandle.IsValid() || StartTime != 0) {
		StopRecording();
		UE_LOG(LogTemp, Warning, TEXT("Recording already in progress. Stopping previous recording."));
	}
	StartTime = FDateTime::Now().GetTicks();
	this->WorldContext->GetTimerManager().SetTimer(TimerHandle, this, &UPositionRecorder::RecordPlayerPositions, 0.5f, true);
}

void UPositionRecorder::StopRecording() {
	if (TimerHandle.IsValid()) {
		WorldContext->GetTimerManager().ClearTimer(TimerHandle);
	}
}

void UPositionRecorder::FinishedRecording()
{
	StopRecording();
	if (PlaySessionCreate.isPlaying)
	{
		auto Data = GetPositionData();
		UE_LOG(LogTemp, Warning, TEXT("Data size: %d"), Data.Num());
		// for (int i = 0; i < Data.Num(); i++) {
		// 	for (int j = 0; j < Data[i].Num(); j++) {
		// 		UE_LOG(LogTemp, Warning, TEXT("Player %d: X=%f, Y=%f, Z=%f"), Data[i][j].Player, Data[i][j].X, Data[i][j].Y, Data[i][j].Z);
		// 	}
		// }
		int PlayerCount = WorldContext->GetNumPlayerControllers();
		Client.CreatePositionsPost(
			PlaySessionCreate.projectId,
			PlaySessionCreate.sessionId,
			PlayerCount,
			Data.Num(),
			Data,
			[this]() {
				UE_LOG(LogTemp, Log, TEXT("Positions sent successfully."));
				Client.FinishedSession(
					PlaySessionCreate.projectId,
					PlaySessionCreate.sessionId,
					[this](FPlaySessionCreate PlaySession) {
						UE_LOG(LogTemp, Log, TEXT("Session finished successfully."));
					}
				);
			}
		);
	}
}

void UPositionRecorder::RecordPlayerPositions() {
	if (!WorldContext || PlaySessionCreate.sessionId == 0) {
		UE_LOG(LogTemp, Warning, TEXT("World context not set."));
		return;
	}
	TArray<FPlayerPosition> CurrentPositions;

	for (FConstPlayerControllerIterator Iterator = WorldContext->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		if (const APlayerController* PC = Iterator->Get()) {
			if (APawn* Pawn = PC->GetPawn()) {
				FVector Location = Pawn->GetActorLocation();
				FPlayerPosition Position;
				Position.Player = PC->GetUniqueID();
				Position.X = Location.X;
				Position.Y = Location.Y;
				Position.Z = Location.Z;
				Position.OffsetTimestamp = (FDateTime::Now().GetTicks() - StartTime) / 10000;

				UE_LOG(LogTemp, Warning, TEXT("Player %d: X=%f, Y=%f, Z=%f time=%d"), Position.Player, Position.X, Position.Y, Position.Z, Position.OffsetTimestamp);
				CurrentPositions.Add(Position);
			}
		}
	}
	PositionData.Add(CurrentPositions);
}

const TArray<TArray<FPlayerPosition>>& UPositionRecorder::GetPositionData() const {
	return PositionData;
}