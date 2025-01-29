#pragma once
#include "OpenAPIHeatMapTaskResultListItem.h"

struct LUDISCANPLUGIN_API FHeatmapData
{
	
	float X;

	float Y;

	float Z;

	float Density;

	FHeatmapData(): X(0), Y(0), Z(0), Density(0)
	{
	}

	static FHeatmapData ParseFromOpenAPIHeatMapTaskResultListItem(const OpenAPI::OpenAPIHeatMapTaskResultListItem& Item)
	{
		FHeatmapData Data;
		Data.X = Item.X;
		Data.Y = Item.Y;
		Data.Z = Item.Z ? Item.Z.GetValue() : 0;
		Data.Density = Item.Density;
		return Data;
	}
};
