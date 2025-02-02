/**
 * API Documentation
 * The API description
 *
 * OpenAPI spec version: 0.0.1
 * 
 *
 * NOTE: This class is auto generated by OpenAPI Generator
 * https://github.com/OpenAPITools/openapi-generator
 * Do not edit the class manually.
 */

#pragma once

#include "OpenAPIBaseModel.h"
#include "OpenAPIV0Api.h"

#include "OpenAPIHelpers.h"
#include "OpenAPICalcFieldRequestDto.h"
#include "OpenAPICalcFieldResponseDto.h"
#include "OpenAPICalcNumberFieldDto.h"
#include "OpenAPICreateHeatmapDto.h"
#include "OpenAPICreatePlaySessionDto.h"
#include "OpenAPICreateProjectDto.h"
#include "OpenAPICreateUserDto.h"
#include "OpenAPIDefaultErrorResponse.h"
#include "OpenAPIDefaultSuccessResponse.h"
#include "OpenAPIHeatmapTaskDto.h"
#include "OpenAPIPlayPositionLogDto.h"
#include "OpenAPIPlaySessionResponseDto.h"
#include "OpenAPIProjectResponseDto.h"
#include "OpenAPIUpdatePlaySessionDto.h"

namespace OpenAPI
{

/* Create heatmap calculation task for project

*/
class OPENAPI_API OpenAPIV0Api::HeatmapControllerCreateProjectTaskRequest : public Request
{
public:
    virtual ~HeatmapControllerCreateProjectTaskRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	OpenAPICreateHeatmapDto OpenAPICreateHeatmapDto;
};

class OPENAPI_API OpenAPIV0Api::HeatmapControllerCreateProjectTaskResponse : public Response
{
public:
    virtual ~HeatmapControllerCreateProjectTaskResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIHeatmapTaskDto Content;
};

/* Create heatmap calculation task

*/
class OPENAPI_API OpenAPIV0Api::HeatmapControllerCreateSessionTaskRequest : public Request
{
public:
    virtual ~HeatmapControllerCreateSessionTaskRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	int32 SessionId = 0;
	OpenAPICreateHeatmapDto OpenAPICreateHeatmapDto;
};

class OPENAPI_API OpenAPIV0Api::HeatmapControllerCreateSessionTaskResponse : public Response
{
public:
    virtual ~HeatmapControllerCreateSessionTaskResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIHeatmapTaskDto Content;
};

/* Get heatmap calculation task

*/
class OPENAPI_API OpenAPIV0Api::HeatmapControllerGetTaskRequest : public Request
{
public:
    virtual ~HeatmapControllerGetTaskRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 TaskId = 0;
};

class OPENAPI_API OpenAPIV0Api::HeatmapControllerGetTaskResponse : public Response
{
public:
    virtual ~HeatmapControllerGetTaskResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIHeatmapTaskDto Content;
};

/* Calculate number field

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerCalcNumberFieldRequest : public Request
{
public:
    virtual ~PlaySessionControllerCalcNumberFieldRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	OpenAPICalcFieldRequestDto OpenAPICalcFieldRequestDto;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerCalcNumberFieldResponse : public Response
{
public:
    virtual ~PlaySessionControllerCalcNumberFieldResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPICalcNumberFieldDto Content;
};

/* Create a play session

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerCreateRequest : public Request
{
public:
    virtual ~PlaySessionControllerCreateRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	OpenAPICreatePlaySessionDto OpenAPICreatePlaySessionDto;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerCreateResponse : public Response
{
public:
    virtual ~PlaySessionControllerCreateResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIPlaySessionResponseDto Content;
};

/* Delete a play session

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerDeleteRequest : public Request
{
public:
    virtual ~PlaySessionControllerDeleteRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	int32 SessionId = 0;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerDeleteResponse : public Response
{
public:
    virtual ~PlaySessionControllerDeleteResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    
};

/* Delete all play sessions

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerDeleteAllRequest : public Request
{
public:
    virtual ~PlaySessionControllerDeleteAllRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerDeleteAllResponse : public Response
{
public:
    virtual ~PlaySessionControllerDeleteAllResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIPlaySessionResponseDto Content;
};

/* Get all play sessions

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerFindAllRequest : public Request
{
public:
    virtual ~PlaySessionControllerFindAllRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	TOptional<int32> Limit;
	TOptional<int32> Offset;
	TOptional<bool> IsFinished;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerFindAllResponse : public Response
{
public:
    virtual ~PlaySessionControllerFindAllResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    TArray<OpenAPIPlaySessionResponseDto> Content;
};

/* Get a play session

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerFindOneRequest : public Request
{
public:
    virtual ~PlaySessionControllerFindOneRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	int32 SessionId = 0;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerFindOneResponse : public Response
{
public:
    virtual ~PlaySessionControllerFindOneResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIPlaySessionResponseDto Content;
};

/* Finish a play session

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerFinishRequest : public Request
{
public:
    virtual ~PlaySessionControllerFinishRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	int32 SessionId = 0;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerFinishResponse : public Response
{
public:
    virtual ~PlaySessionControllerFinishResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIPlaySessionResponseDto Content;
};

/* Update a play session

*/
class OPENAPI_API OpenAPIV0Api::PlaySessionControllerUpdateRequest : public Request
{
public:
    virtual ~PlaySessionControllerUpdateRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	int32 SessionId = 0;
	OpenAPIUpdatePlaySessionDto OpenAPIUpdatePlaySessionDto;
};

class OPENAPI_API OpenAPIV0Api::PlaySessionControllerUpdateResponse : public Response
{
public:
    virtual ~PlaySessionControllerUpdateResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIPlaySessionResponseDto Content;
};

/* Get player position logs

*/
class OPENAPI_API OpenAPIV0Api::PlayerPositionLogControllerGetRequest : public Request
{
public:
    virtual ~PlayerPositionLogControllerGetRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	int32 SessionId = 0;
};

class OPENAPI_API OpenAPIV0Api::PlayerPositionLogControllerGetResponse : public Response
{
public:
    virtual ~PlayerPositionLogControllerGetResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    TArray<OpenAPIPlayPositionLogDto> Content;
};

/* Upload binary player data

*/
class OPENAPI_API OpenAPIV0Api::PlayerPositionLogControllerPostRequest : public Request
{
public:
    virtual ~PlayerPositionLogControllerPostRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 ProjectId = 0;
	int32 SessionId = 0;
	TOptional<HttpFileInput> File;
};

class OPENAPI_API OpenAPIV0Api::PlayerPositionLogControllerPostResponse : public Response
{
public:
    virtual ~PlayerPositionLogControllerPostResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIDefaultSuccessResponse Content;
};

/* Create a project

*/
class OPENAPI_API OpenAPIV0Api::ProjectsControllerCreateRequest : public Request
{
public:
    virtual ~ProjectsControllerCreateRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	OpenAPICreateProjectDto OpenAPICreateProjectDto;
};

class OPENAPI_API OpenAPIV0Api::ProjectsControllerCreateResponse : public Response
{
public:
    virtual ~ProjectsControllerCreateResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIDefaultSuccessResponse Content;
};

/* Delete a project

*/
class OPENAPI_API OpenAPIV0Api::ProjectsControllerDeleteRequest : public Request
{
public:
    virtual ~ProjectsControllerDeleteRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 Id = 0;
};

class OPENAPI_API OpenAPIV0Api::ProjectsControllerDeleteResponse : public Response
{
public:
    virtual ~ProjectsControllerDeleteResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPIDefaultSuccessResponse Content;
};

/* Get all projects

*/
class OPENAPI_API OpenAPIV0Api::ProjectsControllerFindAllRequest : public Request
{
public:
    virtual ~ProjectsControllerFindAllRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	TOptional<int32> Limit;
	TOptional<int32> Offset;
};

class OPENAPI_API OpenAPIV0Api::ProjectsControllerFindAllResponse : public Response
{
public:
    virtual ~ProjectsControllerFindAllResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    TArray<OpenAPIProjectResponseDto> Content;
};

/* List meta data keys

*/
class OPENAPI_API OpenAPIV0Api::ProjectsControllerGetMetaDataKeysRequest : public Request
{
public:
    virtual ~ProjectsControllerGetMetaDataKeysRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 Id = 0;
};

class OPENAPI_API OpenAPIV0Api::ProjectsControllerGetMetaDataKeysResponse : public Response
{
public:
    virtual ~ProjectsControllerGetMetaDataKeysResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    OpenAPICalcFieldResponseDto Content;
};

/* ユーザーを作成

*/
class OPENAPI_API OpenAPIV0Api::UsersControllerCreateRequest : public Request
{
public:
    virtual ~UsersControllerCreateRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	OpenAPICreateUserDto OpenAPICreateUserDto;
};

class OPENAPI_API OpenAPIV0Api::UsersControllerCreateResponse : public Response
{
public:
    virtual ~UsersControllerCreateResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    TSharedPtr<FJsonObject> Content;
};

/* すべてのユーザーを取得

*/
class OPENAPI_API OpenAPIV0Api::UsersControllerFindAllRequest : public Request
{
public:
    virtual ~UsersControllerFindAllRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

};

class OPENAPI_API OpenAPIV0Api::UsersControllerFindAllResponse : public Response
{
public:
    virtual ~UsersControllerFindAllResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    TArray<TSharedPtr<FJsonObject>> Content;
};

/* 特定のユーザーを取得

*/
class OPENAPI_API OpenAPIV0Api::UsersControllerFindOneRequest : public Request
{
public:
    virtual ~UsersControllerFindOneRequest() {}
	void SetupHttpRequest(const FHttpRequestRef& HttpRequest) const final;
	FString ComputePath() const final;

	int32 Id = 0;
};

class OPENAPI_API OpenAPIV0Api::UsersControllerFindOneResponse : public Response
{
public:
    virtual ~UsersControllerFindOneResponse() {}
	void SetHttpResponseCode(EHttpResponseCodes::Type InHttpResponseCode) final;
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;

    TSharedPtr<FJsonObject> Content;
};

}
