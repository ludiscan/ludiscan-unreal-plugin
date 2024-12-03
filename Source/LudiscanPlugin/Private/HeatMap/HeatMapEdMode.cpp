// Fill out your copyright notice in the Description page of Project Settings.


#include "HeatMapEdMode.h"

#include "Client/LudiscanClient.h"

const FEditorModeID FHeatMapEdMode::EM_HeatMapEdMode = TEXT("EM_HeatMapEdMode");

FHeatMapEdMode::FHeatMapEdMode()
{
	const float SavedFilter = LudiscanClient::GetSaveHeatmapColorScaleFilter(1.0f);
	SetColorScaleFactor(SavedFilter);
	const bool SavedZAxis = LudiscanClient::GetSaveHeatmapDrawZAxis(false);
	SetDrawZAxis(SavedZAxis);
}

FHeatMapEdMode::~FHeatMapEdMode()
{
}


void FHeatMapEdMode::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEdMode::Draw(View, PDI);
	// 描画位置と色をもとに点を描画
	for (const TPair<FVector, FColor>& Data : DrawPositions)
	{
		if (!View->ViewFrustum.IntersectBox(Data.Key, FVector(20.0f, 20.0f, 20.0f)) || Data.Value.A == 0)  // ビューフラスタムとの交差判定
		{
			continue;
		}
		PDI->DrawPoint(Data.Key, Data.Value, 20.0f, SDPG_Foreground);
	}
}

void FHeatMapEdMode::SetHeatmapData(const TArray<FPlaySessionHeatmapResponseDto>& NewHeatmapData)
{
	// 渡されたヒートマップデータを格納
	HeatmapArray = NewHeatmapData;

	CalculateBoundingBox();
}

void FHeatMapEdMode::CalculateBoundingBox()
{
	// 初期値は非常に大きな値
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
	float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
	MaxDensityValue = 0.0f;
	TotalDensity = 0.0f;
	float Space = 100.0f;

	// HeatmapArrayからmin, maxを計算
	for (const FPlaySessionHeatmapResponseDto& Data : HeatmapArray)
	{
		minX = FMath::Min(minX, Data.X);
		minY = FMath::Min(minY, Data.Y);
		minZ = FMath::Min(minZ, Data.Z);

		maxX = FMath::Max(maxX, Data.X);
		maxY = FMath::Max(maxY, Data.Y);
		maxZ = FMath::Max(maxZ, Data.Z);
		MaxDensityValue = FMath::Max(MaxDensityValue, Data.Density);
		TotalDensity += Data.Density;
	}

	// 計算したmin, maxをBoundingBoxに設定
	BoundingBox.Min = FVector(minX - Space, minY - Space, minZ - Space);
	BoundingBox.Max = FVector(maxX + Space, maxY + Space, maxZ + Space);
}
void FHeatMapEdMode::Enter()
{
	FEdMode::Enter();
	// 必要に応じて初期化コードを追加
}

void FHeatMapEdMode::Exit()
{
	// 必要に応じてクリーンアップコードを追加
	FEdMode::Exit();
}

void FHeatMapEdMode::GenerateDrawPositions()
{
	DrawPositions.Empty();
	const float StepSize = 100.0f;
	const FVector BoxSize = BoundingBox.Max - BoundingBox.Min;
	const int32 StepsX = FMath::CeilToInt(BoxSize.X / StepSize);
	const int32 StepsY = FMath::CeilToInt(BoxSize.Y / StepSize);
	const int32 StepsZ = FMath::CeilToInt(BoxSize.Z / StepSize);

	const float MinDensityValue = 1.0f; // 最小密度値（対数スケール用に1以上を設定）
	const float DensityScaleFactor = FMath::Loge(MaxDensityValue + 1.0f) - FMath::Loge(MinDensityValue);

	for (int32 X = 0; X <= StepsX; ++X)
	{
		for (int32 Y = 0; Y <= StepsY; ++Y)
		{
			for (int32 Z = 0; Z <= StepsZ; ++Z)
			{
				if (!bDrawZAxis && Z != 0)
				{
					continue;
				}

				FVector Position = BoundingBox.Min + FVector(X * StepSize + StepSize / 2, Y * StepSize + StepSize / 2, Z * StepSize / 2);
				float MinDistance = FLT_MAX;
				float ClosestDensity = 0.0f;

				for (const FPlaySessionHeatmapResponseDto& Data : HeatmapArray)
				{
					float Distance = FVector::Dist(Position, FVector(Data.X, Data.Y, bDrawZAxis ? Data.Z : 0.0f));
					if (Distance < MinDistance)
					{
						MinDistance = Distance;
						ClosestDensity = Data.Density;
					}
				}

				// 対数スケールと色強調度で色を計算
				float NormalizedDensity = FMath::Loge(ClosestDensity + 1.0f) / DensityScaleFactor;
				NormalizedDensity = FMath::Clamp(NormalizedDensity * ColorScaleFactor, 0.0f, 1.0f); // 強調度を適用し、範囲を0～1に制限
				FColor Color = FColor::MakeRedToGreenColorFromScalar(NormalizedDensity);
				Color.A = FMath::Clamp(static_cast<int32>(NormalizedDensity * 255), 0, 255);

				// 描画データに追加
				DrawPositions.Add({Position, Color});
			}
		}
	}
}