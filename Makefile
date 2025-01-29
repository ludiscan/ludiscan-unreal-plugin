gen:
	openapi-generator-cli generate -i https://yuhi.tokyo/api-json -g cpp-ue4 -o Source/OpenAPI --additional-properties=enumUnknownDefaultCase=true,