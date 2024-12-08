// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EdMode.h"
#include "Client/LudiscanClient.h"

/**
 * 
 */
class FHeatMapEdMode : public FEdMode
{
public:
	/** エディタモードID */
	const static FEditorModeID EM_HeatMapEdMode;

	FHeatMapEdMode();
	~FHeatMapEdMode();

	// FEdModeのオーバーライドメソッド
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

	void SetHeatmapData(const TArray<FHeatmapData>& NewHeatmapData);

	float GetColorScaleFactor() const { return ColorScaleFactor; }
	void SetColorScaleFactor(float NewValue)
	{
		ColorScaleFactor = FMath::Clamp(NewValue, 0.1f, 10.0f);
		LudiscanClient::SetSaveHeatmapColorScaleFilter(ColorScaleFactor);
	}
	void RefreshDrawPositions() { GenerateDrawPositions(); }

	bool IsDrawZAxis() const { return bDrawZAxis; }
	void SetDrawZAxis(bool bNewValue)
	{
		bDrawZAxis = bNewValue;
		LudiscanClient::SetSaveHeatmapDrawZAxis(bNewValue);
	}
private:
	void GenerateDrawPositions();
	void CalculateBoundingBox();
	FBox BoundingBox;
	float MaxDensityValue = 30.0f;
	float TotalDensity = 0.0f;
	float MaxDistance = 100.0f;
	float PointSize = 5.0f;
	bool bDrawZAxis = false;
	float ColorScaleFactor = 1.0f;

	TArray<FHeatmapData> HeatmapArray;

	TArray<TPair<FVector, FColor>> DrawPositions;
};
