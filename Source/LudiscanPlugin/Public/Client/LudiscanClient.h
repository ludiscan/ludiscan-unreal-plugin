#pragma once

#include "CoreMinimal.h"
#include "FHeatMapTask.h"
#include "FPlaySession.h"
#include "Project.h"
#include "Interfaces/IHttpRequest.h"
#include "HttpModule.h"
#include "SettingsManager.h"
#include "Interfaces/IHttpResponse.h"

struct FPlayerPosition {
    int32_t Player; // プレイヤー識別子
    float X;
    float Y;
    float Z; // オプション（ここでは必須と仮定）
    uint64_t OffsetTimestamp;
};

class LudiscanClient {
public:
    const static FString SaveApiHostNameKey;
    const static FString SaveHeatmapColorScaleFilterKey;
    const static FString SaveHeatmapDrawZAxisKey;
	const static FString SaveProjectIdKey;
	const static FString SaveHeatmapDrawStepSizeKey;
    LudiscanClient();
    ~LudiscanClient();

    void SetConfig(FString OverrideHostName);

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
        TFunction<void()> OnSuccess = []() {}
    );

    void FinishedSession(
        int projectId,
        int sessionId,
        TFunction<void(FPlaySession)> OnSuccess = [](FPlaySession PlaySession) {}
    );

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

    void GetProjects(
        TFunction<void(TArray<FProject>)> OnSuccess,
        TFunction<void(FString)> OnFailure = [](FString Message) {}
    ) const;

    void GetSessions(
        int projectId,
        TFunction<void(TArray<FPlaySession>)> OnSuccess,
        TFunction<void(FString)> OnFailure = [](FString Message) {}
    ) const;

private:

    FString bApiHostName;
    bool bIsDebug = false;

    static TArray<uint8> ConstructBinaryData(int players, int stampCount, const TArray<TArray<FPlayerPosition>>& allPositions);

    static TSharedRef<IHttpRequest> CreateHttpContent(const TArray<uint8>& BinaryData);
};

inline const FString LudiscanClient::SaveApiHostNameKey = TEXT("LudiscanApiHostName");
inline const FString LudiscanClient::SaveHeatmapColorScaleFilterKey = TEXT("LudiscanHeatmapColorScaleFiler");
inline const FString LudiscanClient::SaveHeatmapDrawZAxisKey = TEXT("LudiscanHeatmapDrawZAxis");
inline const FString LudiscanClient::SaveProjectIdKey = TEXT("LudiscanProjectId");
inline const FString LudiscanClient::SaveHeatmapDrawStepSizeKey = TEXT("LudiscanHeatmapDrawStepSize");

inline LudiscanClient::LudiscanClient()
{
	bApiHostName = GetSaveApiHostName("https://yuhi.tokyo");
	bIsDebug = true;
}

inline LudiscanClient::~LudiscanClient()
{
	
}

inline void LudiscanClient::SetConfig(FString OverrideHostName)
{
	bApiHostName = OverrideHostName;
}

inline void LudiscanClient::SetSaveApiHostName(const FString& NewHostName)
{
	FSettingsManager::SetString(SaveApiHostNameKey, NewHostName);
}

inline FString LudiscanClient::GetSaveApiHostName(const FString& DefaultValue)
{
	return FSettingsManager::GetString(SaveApiHostNameKey, DefaultValue);
}

inline void LudiscanClient::SetSaveHeatmapColorScaleFilter(const float NewColorScaleFilter)
{
	FSettingsManager::SetFloat(SaveHeatmapColorScaleFilterKey, NewColorScaleFilter);
}

inline float LudiscanClient::GetSaveHeatmapColorScaleFilter(const float DefaultColorScaleFilter)
{
	return FSettingsManager::GetFloat(SaveHeatmapColorScaleFilterKey, DefaultColorScaleFilter);
}

inline void LudiscanClient::SetSaveHeatmapDrawZAxis(const bool NewDrawZAxis)
{
	FSettingsManager::SetBool(SaveHeatmapDrawZAxisKey, NewDrawZAxis);
}

inline bool LudiscanClient::GetSaveHeatmapDrawZAxis(const bool DefaultDrawZAxis)
{
	return FSettingsManager::GetBool(SaveHeatmapDrawZAxisKey, DefaultDrawZAxis);
}

inline void LudiscanClient::SetSaveProjectId(const int NewProjectId)
{
	FSettingsManager::SetInt(SaveProjectIdKey, NewProjectId);
}

inline int LudiscanClient::GetSaveProjectId(const int DefaultProjectId)
{
	return FSettingsManager::GetInt(SaveProjectIdKey, DefaultProjectId);
}

inline void LudiscanClient::SetSaveHeatmapDrawStepSize(const int NewStepSize)
{
	FSettingsManager::SetInt(SaveHeatmapDrawStepSizeKey, NewStepSize);
}

inline int LudiscanClient::GetSaveHeatmapDrawStepSize(const int DefaultStepSize)
{
	return FSettingsManager::GetInt(SaveHeatmapDrawStepSizeKey, DefaultStepSize);
}

inline void LudiscanClient::CreatePositionsPost(int projectId, int sessionId, int players, int stampCount,
	const TArray<TArray<FPlayerPosition>>& allPositions, TFunction<void()> OnSuccess)
{
	TArray<uint8> BinaryData = ConstructBinaryData(players, stampCount, allPositions);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateHttpContent(BinaryData);
       
	Request->SetURL(bApiHostName + "/api/v0/projects/" + FString::FromInt(projectId) + "/play_session/" + FString::FromInt(sessionId) + "/player_position_log");
	Request->SetVerb("POST");

	if (bIsDebug)
	{
	}
	// 完了時のログ出力
	Request->OnProcessRequestComplete().BindLambda([OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid()) {
			UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *Response->GetContentAsString());
			OnSuccess();
		} else {
			UE_LOG(LogTemp, Error, TEXT("Request failed"));
		}
	});


	Request->ProcessRequest();
}

inline void LudiscanClient::FinishedSession(int projectId, int sessionId, TFunction<void(FPlaySession)> OnSuccess)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(bApiHostName + "/api/v0/projects/" + FString::FromInt(projectId) + "/play_session/" + FString::FromInt(sessionId) + "/finish");
	Request->SetVerb("POST");

	Request->OnProcessRequestComplete().BindLambda([this, OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid()) {
			FString ResponseContent = Response->GetContentAsString();
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
			if (TSharedPtr<FJsonObject> JsonObject; FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) {
				if (FPlaySession PlaySession; FPlaySession::ParseDataFromJson(JsonObject, PlaySession)) {
					OnSuccess(PlaySession);
				}
			}
		}
	});
	Request->ProcessRequest();
}

inline void LudiscanClient::CreateSessionHeatMap(
	int ProjectId,
	int SessionId,
	TFunction<void(FHeatMapTask)> OnSuccess,
	TFunction<void(FString)> OnFailure,
	int StepSize,
	bool ZVisualize) const
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	const FString Url = bApiHostName + FString::Printf(TEXT("/api/v0/heatmap/projects/%d/play_session/%d/tasks"), ProjectId, SessionId);
	Request->SetURL(Url);
	Request->SetVerb("POST");
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetNumberField("stepSize", StepSize);
	JsonObject->SetBoolField("zVisible", ZVisualize);
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);
	Request->SetHeader("Content-Type", "application/json");
	// 完了時のログ出力
	Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid())
		{
			// レスポンスを文字列として取得
			FString ResponseContent = Response->GetContentAsString();

			// レスポンスのJSONを解析
			FHeatMapTask Task;
                
			if (FHeatMapTask::ParseDataFromJson(ResponseContent, Task))
			{
				OnSuccess(Task);
				return;
			}
			UE_LOG(LogTemp, Error, TEXT("Failed to deserialize the JSON response."));
			OnFailure("Failed to deserialize the JSON response.");
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("Request failed"));
		return OnFailure("Request failed");
	});


	Request->ProcessRequest();
}

inline void LudiscanClient::CreateProjectHeatMap(
	int ProjectId,
	TFunction<void(FHeatMapTask)> OnSuccess,
	TFunction<void(FString)> OnFailure,
	int StepSize,
	bool ZVisualize) const
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	const FString Url = bApiHostName + FString::Printf(TEXT("/api/v0/heatmap/projects/%d/tasks"), ProjectId);
	Request->SetURL(Url);
	Request->SetVerb("POST");
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetNumberField("stepSize", StepSize);
	JsonObject->SetBoolField("zVisible", ZVisualize);
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);
	Request->SetHeader("Content-Type", "application/json");
	// 完了時のログ出力
	Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid())
		{
			// レスポンスを文字列として取得
			FString ResponseContent = Response->GetContentAsString();

			// レスポンスのJSONを解析
			FHeatMapTask Task;
                
			if (FHeatMapTask::ParseDataFromJson(ResponseContent, Task))
			{
				OnSuccess(Task);
				return;
			}
			UE_LOG(LogTemp, Error, TEXT("Failed to deserialize the JSON response."));
			OnFailure("Failed to deserialize the JSON response.");
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("Request failed"));
		return OnFailure("Request failed");
	});


	Request->ProcessRequest();
}


inline void LudiscanClient::GetTask(
	const FHeatMapTask& Task,
	TFunction<void(FHeatMapTask)> OnSuccess,
	TFunction<void(FString)> OnFailure) const
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	const FString Url = bApiHostName + FString::Printf(TEXT("/api/v0/heatmap/tasks/%d"), Task.TaskId);
	Request->SetURL(Url);
	Request->SetVerb("GET");

	// 完了時のログ出力
	Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid())
		{
			// レスポンスを文字列として取得
			FString ResponseContent = Response->GetContentAsString();

			// レスポンスのJSONを解析
			FHeatMapTask Task;
                
			if (FHeatMapTask::ParseDataFromJson(ResponseContent, Task))
			{
				OnSuccess(Task);
				return;
			}
			UE_LOG(LogTemp, Error, TEXT("Failed to deserialize the JSON response."));
			OnFailure("Failed to deserialize the JSON response.");
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("Request failed"));
		return OnFailure("Request failed");
	});


	Request->ProcessRequest();
}


inline void LudiscanClient::CreateSession(
	int projectId,
	const FString& Name,
	const FString& DeviceId,
	const FString& Platform,
	const FString& AppVersion,
	const FString& levelName,
	TMap<FString, FString> ExtraData,
	TFunction<void(FPlaySession)> OnResponse) const
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(bApiHostName + "/api/v0/projects/" + FString::FromInt(projectId) + "/play_session");
	Request->SetVerb("POST");
	// JSONデータの作成
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("name", Name);
	JsonObject->SetStringField("deviceId", DeviceId);
	JsonObject->SetStringField("platform", Platform);
	JsonObject->SetStringField("appVersion", AppVersion);

	TSharedPtr<FJsonObject> MetaData = MakeShareable(new FJsonObject());
	MetaData->SetStringField("mapName", levelName);
	for (const auto& Pair : ExtraData)
	{
		MetaData->SetStringField(Pair.Key, Pair.Value);
	}

	JsonObject->SetObjectField("metaData", MetaData);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// リクエストボディ設定
	Request->SetContentAsString(RequestBody);
	Request->SetHeader("Content-Type", "application/json");

	Request->OnProcessRequestComplete().BindLambda([OnResponse, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid())
		{
			FString ResponseContent = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				if (FPlaySession PlaySession; FPlaySession::ParseDataFromJson(JsonObject, PlaySession))
				{
					OnResponse(PlaySession);
				}
			}
		}
	});
	Request->ProcessRequest();
}

inline void LudiscanClient::GetProjects(TFunction<void(TArray<FProject>)> OnSuccess, TFunction<void(FString)> OnFailure) const
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(bApiHostName + "/api/v0/projects/");
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid())
		{
			FString ResponseContent = Response->GetContentAsString();
			TArray<FProject> Projects;
			if (FProject::ParseArrayFromJson(ResponseContent, Projects))
			{
				OnSuccess(Projects);
				return;
			}
			OnFailure("Failed to parse JSON response.");
			return;
		}
		OnFailure("Request failed.");
	});
	Request->ProcessRequest();
}

inline void LudiscanClient::GetSessions(int projectId, TFunction<void(TArray<FPlaySession>)> OnSuccess,
	TFunction<void(FString)> OnFailure) const
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(bApiHostName + FString::Printf(TEXT("/api/v0/projects/%d/play_session?limit=%d&offset=%d"), projectId, 30, 0));
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		UE_LOG(LogTemp, Log, TEXT("Request completed"));
		if (bWasSuccessful && Response.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Request succeeded"));
			FString ResponseContent = Response->GetContentAsString();
			TArray<FPlaySession> PlaySessionData;
			if (FPlaySession::ParseArrayFromJson(ResponseContent, PlaySessionData))
			{
				UE_LOG(LogTemp, Log, TEXT("JSON parsed successfully"));
				OnSuccess(PlaySessionData);
				return;
			}
			OnFailure("Failed to parse JSON response.");
			return;
		}
		OnFailure("Request failed.");
	});
        
	Request->ProcessRequest();
        
}

inline TArray<uint8> LudiscanClient::ConstructBinaryData(int players, int stampCount,
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

inline TSharedRef<IHttpRequest> LudiscanClient::CreateHttpContent(const TArray<uint8>& BinaryData)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// バイナリデータをリクエストに追加
	Request->SetContent(BinaryData);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
        
	// `multipart/form-data`形式でリクエストを構築
	FString Boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
	FString ContentType = FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary);
	Request->SetHeader(TEXT("Content-Type"), *ContentType);

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
	Request->SetContent(PayloadData);
        
	return Request;
}