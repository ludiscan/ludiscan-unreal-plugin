// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"
#include "Client/LudiscanClient.h"
#include "HeatMapEdMode.generated.h"

/**
 * 
 */
UCLASS()
class UHeatMapEdMode : public UBaseLegacyWidgetEdMode
{
	GENERATED_BODY()
public:
	/** エディタモードID */
	const static FEditorModeID EM_HeatMapEdMode;

	// ツール名の定義
	static FString InteractiveToolName;

	UHeatMapEdMode();
	~UHeatMapEdMode();

	// FEdModeのオーバーライドメソッド
	virtual void Enter() override;
	virtual void Exit() override;
	void RegisterTools();
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;

	void SetHeatmapData(const TArray<FHeatmapData>& NewHeatmapData);

	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;

	float GetColorScaleFactor() const
	{
		return ColorScaleFactor;
	}

	void SetColorScaleFactor(float NewValue)
	{
		ColorScaleFactor = NewValue;
		LudiscanAPI::LudiscanClient::SetSaveHeatmapColorScaleFilter(NewValue);
	}
	void RefreshDrawPositions()
	{
		GenerateDrawPositions();
	}

	bool IsDrawZAxis() const
	{
		return DrawZAxis;
	}
	void SetDrawZAxis(bool bNewValue)
	{
		DrawZAxis = bNewValue;
		LudiscanAPI::LudiscanClient::SetSaveHeatmapDrawZAxis(bNewValue);
	}

	int GetDrawStepSize() const
	{
		return DrawStepSize;
	}

	void SetDrawStepSize(int NewValue)
	{
		DrawStepSize = NewValue;
		LudiscanAPI::LudiscanClient::SetSaveHeatmapDrawStepSize(NewValue);
	}

	int GetDrawZOffset() const
	{
		return DrawZOffset;
	}

	void SetDrawZOffset(int NewValue)
	{
		DrawZOffset = NewValue;
	}

	float GetDrawMinDensity() const
	{
		return DrawMinDensity;
	}

	void SetDrawMinDensity(float NewValue)
	{
		DrawMinDensity = NewValue;
	}

	virtual bool UsesToolkits() const override;

	virtual void CreateToolkit() override;
private:
	
	UPROPERTY(EditAnywhere, Category = Options)
	float ColorScaleFactor = 1.0f;

	UPROPERTY(EditAnywhere, Category = Options)
	bool DrawZAxis = false;

	UPROPERTY(EditAnywhere, Category = Options)
	int DrawStepSize = 100;

	UPROPERTY(EditAnywhere, Category = Options)
	int DrawZOffset = 0;

	UPROPERTY(EditAnywhere, Category = Options)
	float DrawMinDensity = 0.0f;

	UPROPERTY(EditAnywhere, Category = Options)
	FBox BoundingBox;

	UPROPERTY(EditAnywhere, Category = Options)
	float MaxDensityValue = 1.0f;

	UPROPERTY(EditAnywhere, Category = Options)
	float MinDensityValue = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = Options)
	float TotalDensity = 0.0f;

	UPROPERTY(EditAnywhere, Category = Options)
	float MaxDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = Options)
	float PointSize = 5.0f;

	TArray<FHeatmapData> HeatmapArray;

	TArray<TPair<FVector, FColor>> DrawPositions;

	void GenerateDrawPositions();
	void CalculateBoundingBox();

};
