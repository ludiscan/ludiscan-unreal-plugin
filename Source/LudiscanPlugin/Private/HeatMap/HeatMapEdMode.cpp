// Fill out your copyright notice in the Description page of Project Settings.


#include "HeatMap/HeatMapEdMode.h"
#include "LudiscanPluginCommands.h"
#include "Client/LudiscanClient.h"

#define LOCTEXT_NAMESPACE "HeatMapEdModeEditorMode"

const FEditorModeID UHeatMapEdMode::EM_HeatMapEdMode = TEXT("EM_HeatMapEdMode");

FString UHeatMapEdMode::InteractiveToolName = TEXT("HeatMap_InteractiveTool");

UHeatMapEdMode::UHeatMapEdMode()
{

	FModuleManager::Get().LoadModule("EditorStyle");

	Info = FEditorModeInfo(UHeatMapEdMode::EM_HeatMapEdMode,
	LOCTEXT("HeatMapMode", "ヒートマップモード"),
	FSlateIcon(),
	true);
}

UHeatMapEdMode::~UHeatMapEdMode()
{
}


// void UHeatMapEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
// {
// 	UBaseLegacyWidgetEdMode::Render(View, Viewport, PDI);
// }

void UHeatMapEdMode::SetHeatmapData(const TArray<FHeatmapData>& NewHeatmapData)
{
	// 渡されたヒートマップデータを格納
	HeatmapArray = NewHeatmapData;
	CalculateBoundingBox();
}

void UHeatMapEdMode::Enter()
{
	UEdMode::Enter();
	// 必要に応じて初期化コードを追加
	// ツールの登録
	RegisterTools();
	float SavedColorScaleFilter = LudiscanClient::GetSaveHeatmapColorScaleFilter(1.0f);
	ColorScaleFactor = SavedColorScaleFilter;
	bool SavedDrawZAxis = LudiscanClient::GetSaveHeatmapDrawZAxis(false);
	DrawZAxis = SavedDrawZAxis;
	int SavedStepSize = LudiscanClient::GetSaveHeatmapDrawStepSize(100);
	DrawStepSize = SavedStepSize;
}

void UHeatMapEdMode::Exit()
{
	SetHeatmapData(TArray<FHeatmapData>());
	UEdMode::Exit();
}

void UHeatMapEdMode::RegisterTools()
{
	// no-op
}

void UHeatMapEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	Super::Render(View, Viewport, PDI);

	// 描画位置と色をもとに点を描画
	for (TPair<FVector, FColor>& Data : DrawPositions)
	{
		if (!View->ViewFrustum.IntersectBox(Data.Key, FVector(20.0f, 20.0f, 20.0f)))  // ビューフラスタムとの交差判定
		{
			continue;
		}
		PDI->DrawPoint(Data.Key, Data.Value, FMath::Min(DrawStepSize, 25), SDPG_Foreground);
	}
}

bool UHeatMapEdMode::UsesToolkits() const
{
	return false;
}

void UHeatMapEdMode::CreateToolkit()
{
	UEdMode::CreateToolkit();
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UHeatMapEdMode::GetModeCommands() const
{
	return FLudiscanPluginCommands::Get().GetCommands();
}

void UHeatMapEdMode::CalculateBoundingBox()
{
	if (HeatmapArray.Num() == 0)
	{
		return;
	}
	// 初期値は非常に大きな値
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
	float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
	MaxDensityValue = 0.0f;
	MinDensityValue = FLT_MAX;
	TotalDensity = 0.0f;
	float Space = 100.0f;

	// HeatmapArrayからmin, maxを計算
	for (const FHeatmapData& Data : HeatmapArray)
	{
		minX = FMath::Min(minX, Data.X);
		minY = FMath::Min(minY, Data.Y);
		minZ = FMath::Min(minZ, Data.Z);

		maxX = FMath::Max(maxX, Data.X);
		maxY = FMath::Max(maxY, Data.Y);
		maxZ = FMath::Max(maxZ, Data.Z);
		MaxDensityValue = FMath::Max(MaxDensityValue, Data.Density);
		MinDensityValue = FMath::Min(MinDensityValue, Data.Density);
		
		TotalDensity += Data.Density;
	}

	// 計算したmin, maxをBoundingBoxに設定
	BoundingBox.Min = FVector(minX - Space, minY - Space, minZ - Space);
	BoundingBox.Max = FVector(maxX + Space, maxY + Space, maxZ + Space);
}

void UHeatMapEdMode::GenerateDrawPositions()
{
	if (HeatmapArray.Num() == 0)
	{
		return;
	}
	DrawPositions.Empty();
	const float StepSize = DrawStepSize;
	const FVector BoxSize = BoundingBox.Max - BoundingBox.Min;
	if (BoxSize.X <= 0.0f || BoxSize.Y <= 0.0f || BoxSize.Z <= 0.0f)
	{
		return;
	}
	const int32 StepsX = FMath::CeilToInt(BoxSize.X / StepSize);
	const int32 StepsY = FMath::CeilToInt(BoxSize.Y / StepSize);
	const int32 StepsZ = FMath::CeilToInt(BoxSize.Z / StepSize);

	const float DensityScaleFactor = FMath::Loge(MaxDensityValue + 1.0f) - FMath::Loge(MinDensityValue);
	if (DensityScaleFactor <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("DensityScaleFactor is non-positive. Skipping GenerateDrawPositions."));
		return;
	}

	for (int32 X = 0; X <= StepsX; ++X)
	{
		for (int32 Y = 0; Y <= StepsY; ++Y)
		{
			for (int32 Z = 0; Z <= StepsZ; ++Z)
			{
				if (!DrawZAxis && Z != 0)
				{
					continue;
				}

				FVector Position = BoundingBox.Min + FVector(X * StepSize + StepSize / 2, Y * StepSize + StepSize / 2, Z * StepSize / 2);
				float MinDistance = FLT_MAX;
				float ClosestDensity = 0.0f;

				for (const FHeatmapData& Data : HeatmapArray)
				{
					float Distance = FVector::Dist(Position, FVector(Data.X, Data.Y, DrawZAxis ? Data.Z : 0.0f));
					if (Distance < MinDistance)
					{
						MinDistance = Distance;
						ClosestDensity = Data.Density;
					}
				}

				if (!DrawMinDensity && ClosestDensity <= MinDensityValue)
				{
					continue;
				}

				// 対数スケールと色強調度で色を計算
				float NormalizedDensity = FMath::Loge(ClosestDensity + 1.0f) / DensityScaleFactor;
				NormalizedDensity = FMath::Clamp(NormalizedDensity * ColorScaleFactor, 0.0f, 1.0f); // 強調度を適用し、範囲を0～1に制限
				FColor Color = FColor::MakeRedToGreenColorFromScalar(NormalizedDensity);
				Color.A = FMath::Clamp(static_cast<int32>(NormalizedDensity * 255), 0, 255);
				if (Color.A < 20)
				{
					Color.A = 0;
				}

				Position.Z += DrawZOffset;

				// 描画データに追加
				DrawPositions.Add({Position, Color});
			}
		}
	}
}
#undef LOCTEXT_NAMESPACE
