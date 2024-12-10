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
	FString SessionTitle,
	TFunction<void(FPlaySession)> OnResponse
)
{
	if (SessionTitle.IsEmpty())
	{
		SessionTitle  = "TitleNone";
	}
	if (OnResponse == nullptr)
	{
		OnResponse = [](FPlaySession PlaySession) {};
	}
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
		SessionTitle,
		DeviceId,
		platform,
		appVersion,
		LevelName,
		extraData,
		[this, OnResponse](FPlaySession PlaySession) {
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

void UPositionRecorder::FinishedSession()
{
	StopRecording();
	if (PlaySessionCreate.bIsPlaying)
	{
		auto Data = GetPositionData();
		UE_LOG(LogTemp, Warning, TEXT("Data size: %d"), Data.Num());
		int PlayerCount = WorldContext->GetNumPlayerControllers();
		Async(EAsyncExecution::Thread, [this, PlayerCount, Data]()
		{
			Client.CreatePositionsPostSync(
			PlaySessionCreate.ProjectId,
			PlaySessionCreate.SessionId,
			PlayerCount,
			Data.Num(),
			Data);
			UE_LOG(LogTemp, Log, TEXT("Positions sent successfully."));
			Client.FinishedSessionSync(
				PlaySessionCreate.ProjectId,
				PlaySessionCreate.SessionId);
			UE_LOG(LogTemp, Log, TEXT("Session finished successfully."));
		});
		
	}
}

void UPositionRecorder::RecordPlayerPositions() {
	if (!WorldContext || PlaySessionCreate.SessionId == 0) {
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

void UPositionRecorder::UpdateSessionData(
	TMap<FString, FString> ExtraData,
	TFunction<void()> OnSuccess
	)
{
	if (PlaySessionCreate.SessionId == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Session not created."));
		return;
	}
	if (ExtraData.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("No data to update."));
		return;
	}
	Client.UpdateSession(
		PlaySessionCreate.ProjectId,
		PlaySessionCreate.SessionId,
		ExtraData,
		[this, OnSuccess](FPlaySession PlaySession) {
			UE_LOG(LogTemp, Log, TEXT("Session updated successfully."));
			OnSuccess();
		}
	);
}

const TArray<TArray<FPlayerPosition>>& UPositionRecorder::GetPositionData() const {
	return PositionData;
}
