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

namespace OpenAPI
{

/*
 * OpenAPICreateUserDto
 *
 * 
 */
class OPENAPI_API OpenAPICreateUserDto : public Model
{
public:
    virtual ~OpenAPICreateUserDto() {}
	bool FromJson(const TSharedPtr<FJsonValue>& JsonValue) final;
	void WriteJson(JsonWriter& Writer) const final;

	FString Name;
	FString Password;
	FString Email;
};

}
