#include "Client/LudiscanClient.h"
#include "OpenAPIV0ApiOperations.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Client/SettingsManager.h"
#include "LudiscanPlugin.h"

using namespace OpenAPI;

namespace LudiscanAPI
{
	
	const FString LudiscanClient::SaveApiHostNameKey = TEXT("LudiscanApiHostName");
	const FString LudiscanClient::SaveHeatmapColorScaleFilterKey = TEXT("LudiscanHeatmapColorScaleFiler");
	const FString LudiscanClient::SaveHeatmapDrawZAxisKey = TEXT("LudiscanHeatmapDrawZAxis");
	const FString LudiscanClient::SaveProjectIdKey = TEXT("LudiscanProjectId");
	const FString LudiscanClient::SaveHeatmapDrawStepSizeKey = TEXT("LudiscanHeatmapDrawStepSize");

	void PlayerPositionLogControllerPostRequestFeature::SetupHttpRequest(const FHttpRequestRef& HttpRequest) const
	{
		HttpRequest->SetVerb(TEXT("POST"));

		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));

		// `multipart/form-data`形式でリクエストを構築
		FString Boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
		FString ContentType = FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary);
		HttpRequest->SetHeader(TEXT("Content-Type"), *ContentType);

		// `file`フィールドとしてバイナリデータを追加
		FString Payload;
		Payload += "--" + Boundary + "\r\n";
		Payload += "Content-Disposition: form-data; name=\"file\"; filename=\"data.bin\"\r\n";
		Payload += "Content-Type: application/octet-stream\r\n\r\n";

		// バイナリデータのペイロードを組み立て
		TArray<uint8> PayloadData;
		PayloadData.Append(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*Payload)), Payload.Len());
		PayloadData.Append(BinaryData);
		FString EndBoundary = "\r\n--" + Boundary + "--\r\n";
		PayloadData.Append(reinterpret_cast<uint8*>(TCHAR_TO_UTF8(*EndBoundary)), EndBoundary.Len());

		// リクエストにペイロードをセット
		HttpRequest->SetContent(PayloadData);
	}

	FString PlayerPositionLogControllerPostRequestFeature::ComputePath() const
	{
		TMap<FString, FStringFormatArg> PathParams = { 
			{ TEXT("project_id"), FStringFormatArg(ToUrlString(ProjectId)) },
			{ TEXT("session_id"), FStringFormatArg(ToUrlString(SessionId)) } };

		FString Path = FString::Format(TEXT("/api/v0/projects/{project_id}/play_session/{session_id}/player_position_log"), PathParams);

		return Path;
	}

	void OpenAPIV0ApiFeature::HandleResponse(FHttpResponsePtr HttpResponse, bool bSucceeded,
		Response& InOutResponse) const
	{
		InOutResponse.SetHttpResponse(HttpResponse);
		InOutResponse.SetSuccessful(bSucceeded);

		if (bSucceeded && HttpResponse.IsValid())
		{
			InOutResponse.SetHttpResponseCode(static_cast<EHttpResponseCodes::Type>(HttpResponse->GetResponseCode()));
			FString ContentType = HttpResponse->GetContentType();
			FString Content;

			if (ContentType.IsEmpty())
			{
				return; // Nothing to parse
			}
			else if (ContentType.StartsWith(TEXT("application/json")) || ContentType.StartsWith("text/json"))
			{
				Content = HttpResponse->GetContentAsString();

				TSharedPtr<FJsonValue> JsonValue;
				auto Reader = TJsonReaderFactory<>::Create(Content);

				if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
				{
					if (InOutResponse.FromJson(JsonValue))
						return; // Successfully parsed
				}
			}
			else if(ContentType.StartsWith(TEXT("text/plain")))
			{
				Content = HttpResponse->GetContentAsString();
				InOutResponse.SetResponseString(Content);
				return; // Successfully parsed
			}

			// Report the parse error but do not mark the request as unsuccessful. Data could be partial or malformed, but the request succeeded.
			UE_LOG(LogLudiscanAPI, Error, TEXT("Failed to deserialize Http response content (type:%s):\n%s"), *ContentType , *Content);
			return;
		}

		// By default, assume we failed to establish connection
		InOutResponse.SetHttpResponseCode(EHttpResponseCodes::RequestTimeout);
	}

	void OpenAPIV0ApiFeature::OnPlayerPositionLogControllerPostResponse(FHttpRequestPtr HttpRequest,
		FHttpResponsePtr HttpResponse, bool bSucceeded,
		OpenAPIV0Api::FPlayerPositionLogControllerPostDelegate Delegate) const
	{
		OpenAPIV0Api::PlayerPositionLogControllerPostResponse Response;
		HandleResponse(HttpResponse, bSucceeded, Response);
		Delegate.ExecuteIfBound(Response);
	}

	FHttpRequestPtr OpenAPIV0ApiFeature::PlayerPositionLogControllerPost(const FString& Url,
		const PlayerPositionLogControllerPostRequestFeature& Request,
		const OpenAPIV0Api::FPlayerPositionLogControllerPostDelegate& Delegate) const
	{
		FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetURL(*(Url + Request.ComputePath()));

		Request.SetupHttpRequest(HttpRequest);

		HttpRequest->OnProcessRequestComplete().BindRaw(this, &OpenAPIV0ApiFeature::OnPlayerPositionLogControllerPostResponse, Delegate);
		HttpRequest->ProcessRequest();
		return HttpRequest;
	}

	LudiscanClient::LudiscanClient()
	{
		Api.SetURL(GetSaveApiHostName("https://yuhi.tokyo"));
		bIsDebug = true;
	}

	LudiscanClient::~LudiscanClient()
	{

	}

	void LudiscanClient::SetConfig(const FString& OverrideHostName)
	{
		Api.SetURL(OverrideHostName);
		SetSaveApiHostName(OverrideHostName);
	}

	void LudiscanClient::SetSaveApiHostName(const FString& NewHostName)
	{
		FSettingsManager::SetString(SaveApiHostNameKey, NewHostName);
	}

	FString LudiscanClient::GetSaveApiHostName(const FString& DefaultValue)
	{
		return FSettingsManager::GetString(SaveApiHostNameKey, DefaultValue);
	}

	void LudiscanClient::SetSaveHeatmapColorScaleFilter(const float NewColorScaleFilter)
	{
		FSettingsManager::SetFloat(SaveHeatmapColorScaleFilterKey, NewColorScaleFilter);
	}

	float LudiscanClient::GetSaveHeatmapColorScaleFilter(const float DefaultColorScaleFilter)
	{
		return FSettingsManager::GetFloat(SaveHeatmapColorScaleFilterKey, DefaultColorScaleFilter);
	}

	void LudiscanClient::SetSaveHeatmapDrawZAxis(const bool NewDrawZAxis)
	{
		FSettingsManager::SetBool(SaveHeatmapDrawZAxisKey, NewDrawZAxis);
	}

	bool LudiscanClient::GetSaveHeatmapDrawZAxis(const bool DefaultDrawZAxis)
	{
		return FSettingsManager::GetBool(SaveHeatmapDrawZAxisKey, DefaultDrawZAxis);
	}

	void LudiscanClient::SetSaveProjectId(const int NewProjectId)
	{
		FSettingsManager::SetInt(SaveProjectIdKey, NewProjectId);
	}

	int LudiscanClient::GetSaveProjectId(const int DefaultProjectId)
	{
		return FSettingsManager::GetInt(SaveProjectIdKey, DefaultProjectId);
	}

	void LudiscanClient::SetSaveHeatmapDrawStepSize(const int NewStepSize)
	{
		FSettingsManager::SetInt(SaveHeatmapDrawStepSizeKey, NewStepSize);
	}

	int LudiscanClient::GetSaveHeatmapDrawStepSize(const int DefaultStepSize)
	{
		return FSettingsManager::GetInt(SaveHeatmapDrawStepSizeKey, DefaultStepSize);
	}

	void LudiscanClient::CreatePositionsPost(int projectId, int sessionId, int players, int stampCount,
		const TArray<TArray<FPlayerPosition>>& allPositions, TFunction<void()> OnSuccess, TFunction<void()> OnFailure)
	{
		if (projectId == 0 || sessionId == 0 || allPositions.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Project ID or Session ID is not set."));
			return;
		}

		PlayerPositionLogControllerPostRequestFeature Req = PlayerPositionLogControllerPostRequestFeature();
		OpenAPIV0Api::PlayerPositionLogControllerPostResponse Res = OpenAPIV0Api::PlayerPositionLogControllerPostResponse();
		OpenAPIV0Api::FPlayerPositionLogControllerPostDelegate Delegate = OpenAPIV0Api::FPlayerPositionLogControllerPostDelegate();
		Req.ProjectId = FString::FromInt(projectId);
		Req.SessionId = FString::FromInt(sessionId);
		Req.BinaryData = ConstructBinaryData(players, stampCount, allPositions);

		OpenAPIV0ApiFeature ApiFeature = OpenAPIV0ApiFeature();
		FString Url = GetSaveApiHostName("https://yuhi.tokyo");
		Delegate.BindLambda([this, OnSuccess](const OpenAPIV0Api::PlayerPositionLogControllerPostResponse& Res)
		{
			if (Res.Content.Success)
			{
				OnSuccess();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create positions."));
			}
		});
		FHttpRequestPtr Ptr = ApiFeature.PlayerPositionLogControllerPost(Url, Req, Delegate);
	}


	void LudiscanClient::FinishedSession(int projectId, int sessionId, TFunction<void(FPlaySession)> OnSuccess, TFunction<void(FString)> OnFailure)
	{
		OpenAPIV0Api::PlaySessionControllerFinishRequest Req = OpenAPIV0Api::PlaySessionControllerFinishRequest();
		OpenAPIV0Api::FPlaySessionControllerFinishDelegate Delegate = OpenAPIV0Api::FPlaySessionControllerFinishDelegate();
		Req.ProjectId = projectId;
		Req.SessionId = sessionId;

		Delegate.BindLambda([this, OnSuccess, OnFailure](const OpenAPIV0Api::PlaySessionControllerFinishResponse& Res)
		{
			if (Res.Content.SessionId != 0)
			{
				OnSuccess(FPlaySession::ParseFromOpenAPIPlaySessionResponseDto(Res.Content));
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to finish session."));
				OnFailure("Failed to finish session.");
			}
		});

		FHttpRequestPtr Ptr = Api.PlaySessionControllerFinish(Req, Delegate);
	}

	void LudiscanClient::CreateSessionHeatMap(
		int ProjectId,
		int SessionId,
		TFunction<void(FHeatMapTask)> OnSuccess,
		TFunction<void(FString)> OnFailure,
		int StepSize,
		bool ZVisualize) const
	{
		OpenAPIV0Api::HeatmapControllerCreateSessionTaskRequest Req = OpenAPIV0Api::HeatmapControllerCreateSessionTaskRequest();
		OpenAPICreateHeatmapDto CreateHeatmapDto = OpenAPICreateHeatmapDto();
		OpenAPIV0Api::FHeatmapControllerCreateSessionTaskDelegate Delegate = OpenAPIV0Api::FHeatmapControllerCreateSessionTaskDelegate();

		CreateHeatmapDto.StepSize = StepSize;
		CreateHeatmapDto.ZVisible = ZVisualize;

		Req.ProjectId = ProjectId;
		Req.SessionId = SessionId;
		Req.OpenAPICreateHeatmapDto = CreateHeatmapDto;

		Delegate.BindLambda([this, OnSuccess, OnFailure](const OpenAPIV0Api::HeatmapControllerCreateSessionTaskResponse& Res)
		{
			if (Res.Content.TaskId != 0)
			{
				OnSuccess(FHeatMapTask::ParseFromOpenAPIHeatmapTaskDto(Res.Content));
			}
			else
			{
				OnFailure("Task not created.");
			}
		});

		FHttpRequestPtr Ptr = Api.HeatmapControllerCreateSessionTask(Req, Delegate);
	}



	void LudiscanClient::CreateProjectHeatMap(
		int ProjectId,
		TFunction<void(FHeatMapTask)> OnSuccess,
		TFunction<void(FString)> OnFailure,
		int StepSize,
		bool ZVisualize) const
	{

		OpenAPIV0Api::HeatmapControllerCreateProjectTaskRequest Req = OpenAPIV0Api::HeatmapControllerCreateProjectTaskRequest();
		OpenAPICreateHeatmapDto CreateHeatmapDto = OpenAPICreateHeatmapDto();

		CreateHeatmapDto.StepSize = StepSize;
		CreateHeatmapDto.ZVisible = ZVisualize;

		Req.ProjectId = ProjectId;
		Req.OpenAPICreateHeatmapDto = CreateHeatmapDto;

		OpenAPIV0Api::FHeatmapControllerCreateProjectTaskDelegate Delegate = OpenAPIV0Api::FHeatmapControllerCreateProjectTaskDelegate();
		Delegate.BindLambda([this, OnSuccess, OnFailure](const OpenAPIV0Api::HeatmapControllerCreateProjectTaskResponse& Res)
		{
			if (Res.Content.TaskId != 0)
			{
				OnSuccess(FHeatMapTask::ParseFromOpenAPIHeatmapTaskDto(Res.Content));
			}
			else
			{
				OnFailure("Task not created.");
			}
		});

		FHttpRequestPtr Ptr = Api.HeatmapControllerCreateProjectTask(Req, Delegate);
	}

	void LudiscanClient::GetTask(
		const FHeatMapTask& Task,
		TFunction<void(FHeatMapTask)> OnSuccess,
		TFunction<void(FString)> OnFailure) const
	{
		OpenAPIV0Api::HeatmapControllerGetTaskRequest Req = OpenAPIV0Api::HeatmapControllerGetTaskRequest();
		OpenAPIV0Api::FHeatmapControllerGetTaskDelegate Delegate = OpenAPIV0Api::FHeatmapControllerGetTaskDelegate();
		Req.TaskId = Task.TaskId;
		Delegate.BindLambda([this, OnSuccess, OnFailure](const OpenAPIV0Api::HeatmapControllerGetTaskResponse& Res)
		{
			if (Res.Content.TaskId != 0)
			{
				OnSuccess(FHeatMapTask::ParseFromOpenAPIHeatmapTaskDto(Res.Content));
			}
			else
			{
				OnFailure("Task not found.");
			}
		});

		FHttpRequestPtr Ptr = Api.HeatmapControllerGetTask(Req, Delegate);
	}
	void LudiscanClient::CreateSession(
		int projectId,
		const FString& Name,
		const FString& DeviceId,
		const FString& Platform,
		const FString& AppVersion,
		const FString& levelName,
		TMap<FString, FString> ExtraData,
		TFunction<void(FPlaySession)> OnResponse) const
	{
		OpenAPICreatePlaySessionDto CreatePlaySessionDto = OpenAPICreatePlaySessionDto();
		OpenAPIV0Api::PlaySessionControllerCreateRequest Req = OpenAPIV0Api::PlaySessionControllerCreateRequest();
		OpenAPIV0Api::FPlaySessionControllerCreateDelegate Delegate = OpenAPIV0Api::FPlaySessionControllerCreateDelegate();

		TSharedPtr<FJsonObject> MetaData = MakeShareable(new FJsonObject());
		MetaData->SetStringField("levelName", levelName);
		for (const auto& Pair : ExtraData)
		{
			MetaData->SetStringField(Pair.Key, Pair.Value);
		}


		CreatePlaySessionDto.Name = Name;
		CreatePlaySessionDto.DeviceId = DeviceId;
		CreatePlaySessionDto.Platform = Platform;
		CreatePlaySessionDto.AppVersion = AppVersion;
		CreatePlaySessionDto.MetaData = MetaData;



		Req.ProjectId = projectId;
		Req.OpenAPICreatePlaySessionDto = CreatePlaySessionDto;



		Delegate.BindLambda([this, OnResponse](const OpenAPIV0Api::PlaySessionControllerCreateResponse& Res)
		{
			if (Res.Content.SessionId != 0)
			{
				OnResponse(FPlaySession::ParseFromOpenAPIPlaySessionResponseDto(Res.Content));
			}
		});

		FHttpRequestPtr Ptr = Api.PlaySessionControllerCreate(Req, Delegate);
	}

	void LudiscanClient::GetProjects(TFunction<void(TArray<FProject>)> OnSuccess, TFunction<void(FString)> OnFailure) const
	{
		OpenAPIV0Api::ProjectsControllerFindAllRequest Req = OpenAPIV0Api::ProjectsControllerFindAllRequest();
		OpenAPIV0Api::FProjectsControllerFindAllDelegate Delegate = OpenAPIV0Api::FProjectsControllerFindAllDelegate();
		Delegate.BindLambda([this, OnFailure, OnSuccess](const OpenAPIV0Api::ProjectsControllerFindAllResponse& Res)
		{
			if (Res.Content.Num() == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Request failed"));
				OnFailure("Request failed");
			}
			else
			{
				TArray<FProject> Projects;
				UE_LOG(LogTemp, Log, TEXT("Request completed with response: %d"), Res.Content.Num());
				for (OpenAPIProjectResponseDto Content : Res.Content)
				{
					Projects.Add(FProject::ParseFromOpenAPIProjectResponseDto(Content));
				} 
				OnSuccess(Projects);
			}
		});
		FHttpRequestPtr Ptr = Api.ProjectsControllerFindAll(Req, Delegate);
	}

	void LudiscanClient::GetSessions(
		int projectId,
		TFunction<void(TArray<FPlaySession>)> OnSuccess,
		TFunction<void(FString)> OnFailure,
		int Limit,
		int Offset) const
	{
		OpenAPIV0Api::PlaySessionControllerFindAllRequest Req = OpenAPIV0Api::PlaySessionControllerFindAllRequest();
		Req.ProjectId = projectId;
		Req.Limit = Limit;
		Req.Offset = Offset;
		Req.IsFinished = true;
		OpenAPIV0Api::FPlaySessionControllerFindAllDelegate Delegate = OpenAPIV0Api::FPlaySessionControllerFindAllDelegate();
		Delegate.BindLambda([this, OnFailure, OnSuccess](const OpenAPIV0Api::PlaySessionControllerFindAllResponse& Res)
		{
			if (Res.Content.Num() == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Session not found."));
				OnFailure("Session not found.");
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Request completed with response: %d"), Res.Content.Num());
				TArray<FPlaySession> PlaySessionData;
				for (const OpenAPIPlaySessionResponseDto& Dto : Res.Content)
				{
					PlaySessionData.Add(FPlaySession::ParseFromOpenAPIPlaySessionResponseDto(Dto));
				}
				OnSuccess(PlaySessionData);
			}
		});
		FHttpRequestPtr Ptr = Api.PlaySessionControllerFindAll(Req, Delegate);
	}

	TArray<uint8> LudiscanClient::ConstructBinaryData(int players, int stampCount,
		const TArray<TArray<FPlayerPosition>>& allPositions)
	{
		TArray<uint8> buffer;

		// メタ情報の追加（playersとstampcount）
		buffer.SetNum(8);
		buffer[0] = players & 0xFF;
		buffer[1] = (players >> 8) & 0xFF;
		buffer[2] = (players >> 16) & 0xFF;
		buffer[3] = (players >> 24) & 0xFF;

		buffer[4] = stampCount & 0xFF;
		buffer[5] = (stampCount >> 8) & 0xFF;
		buffer[6] = (stampCount >> 16) & 0xFF;
		buffer[7] = (stampCount >> 24) & 0xFF;

		// 各タイムスタンプごとのデータを追加
		for (int i = 0; i < stampCount; ++i) {
			for (int p = 0; p < allPositions[1].Num(); ++p) {
				const FPlayerPosition& pos = allPositions[i][p];

				// プレイヤー識別子をリトルエンディアンで追加
				int32_t playerLE = pos.Player;
				uint8* playerPtr = reinterpret_cast<uint8*>(&playerLE);
				buffer.Append(playerPtr, sizeof(int32_t));

				// x, y, zをリトルエンディアンで追加
				float x = pos.X;
				float y = pos.Y;
				float z = pos.Z;
				buffer.Append(reinterpret_cast<uint8*>(&x), sizeof(float));
				buffer.Append(reinterpret_cast<uint8*>(&y), sizeof(float));
				buffer.Append(reinterpret_cast<uint8*>(&z), sizeof(float));

				// offsetTimestampをリトルエンディアンで追加
				uint64_t tsLE = pos.OffsetTimestamp;
				uint8* tsPtr = reinterpret_cast<uint8*>(&tsLE);
				buffer.Append(tsPtr, sizeof(uint64_t));
			}
		}

		return buffer;
	}

	void LudiscanClient::UpdateSession(
		int ProjectId,
		int SessionId,
		TMap<FString, FString> ExtraData,
		TFunction<void(FPlaySession)> OnResponse) const
	{
		OpenAPIV0Api::PlaySessionControllerUpdateRequest Req = OpenAPIV0Api::PlaySessionControllerUpdateRequest();
		OpenAPIUpdatePlaySessionDto RequestDto = OpenAPIUpdatePlaySessionDto();
		TSharedPtr<FJsonObject> MetaData = MakeShareable(new FJsonObject());
		for (const auto& Pair : ExtraData)
		{
			MetaData->SetStringField(Pair.Key, Pair.Value);
		}
		RequestDto.MetaData = MetaData;

		Req.ProjectId = ProjectId;
		Req.SessionId = SessionId;
		Req.OpenAPIUpdatePlaySessionDto = RequestDto;

		OpenAPIV0Api::FPlaySessionControllerUpdateDelegate Delegate = OpenAPIV0Api::FPlaySessionControllerUpdateDelegate();
		Delegate.BindLambda([this, OnResponse](const OpenAPIV0Api::PlaySessionControllerUpdateResponse& Res)
		{
			if (Res.Content.SessionId == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Request failed"));
				return;
			}
			FPlaySession PlaySession = FPlaySession::ParseFromOpenAPIPlaySessionResponseDto(Res.Content);
			OnResponse(PlaySession);
		});

		FHttpRequestPtr Ptr = Api.PlaySessionControllerUpdate(Req, Delegate);

	}
}