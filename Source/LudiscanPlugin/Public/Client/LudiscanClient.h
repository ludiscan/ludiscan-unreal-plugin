#pragma once

#include "CoreMinimal.h"
#include "FHeatMapTask.h"
#include "FPlaySession.h"
#include "Interfaces/IHttpRequest.h"
#include "OpenAPIV0Api.h"


namespace LudiscanAPI
{
	struct LUDISCANPLUGIN_API FPlayerPosition {
		int32_t Player; // プレイヤー識別子
		float X;
		float Y;
		float Z; // オプション（ここでは必須と仮定）
		uint64_t OffsetTimestamp;
	};

	class LUDISCANPLUGIN_API PlayerPositionLogControllerPostRequestFeature : public OpenAPI::Request
	{
	public:
		virtual void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const override final;

		virtual FString ComputePath() const override final;

		FString ProjectId;
		FString SessionId;
		TArray<uint8> BinaryData;
	};

	class LUDISCANPLUGIN_API OpenAPIV0ApiFeature
	{
		void HandleResponse(FHttpResponsePtr HttpResponse, bool bSucceeded, OpenAPI::Response& InOutResponse) const;

		void OnPlayerPositionLogControllerPostResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, OpenAPI::OpenAPIV0Api::FPlayerPositionLogControllerPostDelegate Delegate) const;

	public:
		FHttpRequestPtr PlayerPositionLogControllerPost(const FString& Url, const PlayerPositionLogControllerPostRequestFeature& Request, const OpenAPI::OpenAPIV0Api::FPlayerPositionLogControllerPostDelegate& Delegate = OpenAPI::OpenAPIV0Api::FPlayerPositionLogControllerPostDelegate()) const;
	};

	class LUDISCANPLUGIN_API LudiscanClient {
	public:
		const static FString SaveApiHostNameKey;
		const static FString SaveHeatmapColorScaleFilterKey;
		const static FString SaveHeatmapDrawZAxisKey;
		const static FString SaveProjectIdKey;
		const static FString SaveHeatmapDrawStepSizeKey;
		LudiscanClient();
		~LudiscanClient();

		void SetConfig(const FString& OverrideHostName);

		static void SetSaveApiHostName(const FString& NewHostName);

		static FString GetSaveApiHostName(const FString& DefaultValue);

		static void SetSaveHeatmapColorScaleFilter(const float NewColorScaleFilter);

		static float GetSaveHeatmapColorScaleFilter(const float DefaultColorScaleFilter);

		static void SetSaveHeatmapDrawZAxis(const bool NewDrawZAxis);

		static bool GetSaveHeatmapDrawZAxis(const bool DefaultDrawZAxis);

		static void SetSaveProjectId(const int NewProjectId);

		static int GetSaveProjectId(const int DefaultProjectId);

		static void SetSaveHeatmapDrawStepSize(const int NewStepSize);

		static int GetSaveHeatmapDrawStepSize(const int DefaultStepSize);

		void CreatePositionsPost(
			int projectId,
			int sessionId,
			int players,
			int stampCount,
			const TArray<TArray<FPlayerPosition>>& allPositions,
			TFunction<void()> OnSuccess = []() {},
			TFunction<void()> OnFailure = []() {}
		);

		bool CreatePositionsPostSync(
			int projectId,
			int sessionId,
			int players,
			int stampCount,
			const TArray<TArray<FPlayerPosition>>& allPositions
			)
		{
			if (projectId == 0 || sessionId == 0 || allPositions.Num() == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Project ID or Session ID is not set."));
				return false;
			}
			// 同期処理用のイベントを作成
			FEvent* RequestCompleteEvent = FPlatformProcess::GetSynchEventFromPool(false);
			bool bSuccess = false;
			CreatePositionsPost(
				projectId,
				sessionId,
				players,
				stampCount,
				allPositions,
				[this, &bSuccess, RequestCompleteEvent]()
				{
					bSuccess = true;
					RequestCompleteEvent->Trigger();
				},
				[this, &bSuccess, RequestCompleteEvent]()
				{
					bSuccess = false;
					RequestCompleteEvent->Trigger();
				}
				);
	
			// イベントがトリガーされるまで待機
			RequestCompleteEvent->Wait();
			FPlatformProcess::ReturnSynchEventToPool(RequestCompleteEvent);

	
			return bSuccess;
		}

		void FinishedSession(
			int projectId,
			int sessionId,
			TFunction<void(FPlaySession)> OnSuccess = [](FPlaySession PlaySession) {},
			TFunction<void(FString)> OnFailure = [](FString Message) {}
		);

		FPlaySession FinishedSessionSync(
			int projectId,
			int sessionId
			)
		{
			if (projectId == 0 || sessionId == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Project ID or Session ID is not set."));
				return FPlaySession();
			}
			// 同期処理用のイベントを作成
			FEvent* RequestCompleteEvent = FPlatformProcess::GetSynchEventFromPool(false);
			FPlaySession PlaySession;
			FinishedSession(
				projectId,
				sessionId,
				[&PlaySession, RequestCompleteEvent](FPlaySession Response)
				{
					PlaySession = Response;
					RequestCompleteEvent->Trigger();
				},
				[RequestCompleteEvent](FString Message)
				{
					RequestCompleteEvent->Trigger();
				}
				);
			RequestCompleteEvent->Wait();
			FPlatformProcess::ReturnSynchEventToPool(RequestCompleteEvent);
			return PlaySession;
		}

		void CreateSessionHeatMap(
			int ProjectId,
			int SessionId,
			TFunction<void(FHeatMapTask)> OnSuccess,
			TFunction<void(FString)> OnFailure = [](FString Message) {},
			int StepSize = 300,
			bool ZVisualize = false
		) const;

		void CreateProjectHeatMap(
			int ProjectId,
			TFunction<void(FHeatMapTask)> OnSuccess,
			TFunction<void(FString)> OnFailure = [](FString Message) {},
			int StepSize = 300,
			bool ZVisualize = false
		) const;

		void GetTask(
			const FHeatMapTask& Task,
			TFunction<void(FHeatMapTask)> OnSuccess,
			TFunction<void(FString)> OnFailure = [](FString Message) {}
		) const;

		void CreateSession(
			int projectId,
			const FString& Name,
			const FString& DeviceId,
			const FString& Platform,
			const FString& AppVersion,
			const FString& levelName,
			TMap<FString, FString> ExtraData,
			TFunction<void(FPlaySession)> OnResponse
		) const;

		void UpdateSession(
			int ProjectId,
			int SessionId,
			TMap<FString, FString> ExtraData,
			TFunction<void(FPlaySession)> OnResponse
			) const;

		void GetProjects(
			TFunction<void(TArray<FProject>)> OnSuccess,
			TFunction<void(FString)> OnFailure = [](FString Message) {}
		) const;

		void GetSessions(
			int projectId,
			TFunction<void(TArray<FPlaySession>)> OnSuccess,
			TFunction<void(FString)> OnFailure = [](FString Message) {},
			int Limit = 100,
			int Offset = 0
		) const;

	private:

		OpenAPI::OpenAPIV0Api Api = OpenAPI::OpenAPIV0Api();
		bool bIsDebug = false;

		static TArray<uint8> ConstructBinaryData(int players, int stampCount, const TArray<TArray<FPlayerPosition>>& allPositions);
	};


}
