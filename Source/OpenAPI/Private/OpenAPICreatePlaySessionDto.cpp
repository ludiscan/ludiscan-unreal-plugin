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

#include "OpenAPICreatePlaySessionDto.h"

#include "OpenAPIModule.h"
#include "OpenAPIHelpers.h"

#include "Templates/SharedPointer.h"

namespace OpenAPI
{

void OpenAPICreatePlaySessionDto::WriteJson(JsonWriter& Writer) const
{
	Writer->WriteObjectStart();
	Writer->WriteIdentifierPrefix(TEXT("name")); WriteJsonValue(Writer, Name);
	if (DeviceId.IsSet())
	{
		Writer->WriteIdentifierPrefix(TEXT("deviceId")); WriteJsonValue(Writer, DeviceId.GetValue());
	}
	if (Platform.IsSet())
	{
		Writer->WriteIdentifierPrefix(TEXT("platform")); WriteJsonValue(Writer, Platform.GetValue());
	}
	if (AppVersion.IsSet())
	{
		Writer->WriteIdentifierPrefix(TEXT("appVersion")); WriteJsonValue(Writer, AppVersion.GetValue());
	}
	if (MetaData.IsSet())
	{
		Writer->WriteIdentifierPrefix(TEXT("metaData")); WriteJsonValue(Writer, MetaData.GetValue());
	}
	Writer->WriteObjectEnd();
}

bool OpenAPICreatePlaySessionDto::FromJson(const TSharedPtr<FJsonValue>& JsonValue)
{
	const TSharedPtr<FJsonObject>* Object;
	if (!JsonValue->TryGetObject(Object))
		return false;

	bool ParseSuccess = true;

	ParseSuccess &= TryGetJsonValue(*Object, TEXT("name"), Name);
	ParseSuccess &= TryGetJsonValue(*Object, TEXT("deviceId"), DeviceId);
	ParseSuccess &= TryGetJsonValue(*Object, TEXT("platform"), Platform);
	ParseSuccess &= TryGetJsonValue(*Object, TEXT("appVersion"), AppVersion);
	ParseSuccess &= TryGetJsonValue(*Object, TEXT("metaData"), MetaData);

	return ParseSuccess;
}

}
