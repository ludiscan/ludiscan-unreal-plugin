// Fill out your copyright notice in the Description page of Project Settings.


#include "HeatMap/HeatMapEdMode.h"
#include "LudiscanEditorCommands.h"
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
	float SavedColorScaleFilter = LudiscanAPI::LudiscanClient::GetSaveHeatmapColorScaleFilter(1.0f);
	ColorScaleFactor = SavedColorScaleFilter;
	bool SavedDrawZAxis = LudiscanAPI::LudiscanClient::GetSaveHeatmapDrawZAxis(false);
	DrawZAxis = SavedDrawZAxis;
	int SavedStepSize = LudiscanAPI::LudiscanClient::GetSaveHeatmapDrawStepSize(100);
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
	return FLudiscanEditorCommands::Get().GetCommands();
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

    // 全セルの密度を先に計算して、Min/Maxを取るための配列を用意
    TArray<float> CellDensities;
    CellDensities.SetNum((StepsX+1)*(StepsY+1)*(StepsZ+1));
    // 初期化
    for (float &DensityVal : CellDensities)
    {
        DensityVal = 0.0f;
    }

    // 各セルに対応するインデックス取得用
    auto GetCellIndex = [&](int32 X, int32 Y, int32 Z)
    {
        return X + (StepsX+1)*Y + (StepsX+1)*(StepsY+1)*Z;
    };

    // 各セルの密度計算
    for (int32 X = 0; X <= StepsX; ++X)
    {
        for (int32 Y = 0; Y <= StepsY; ++Y)
        {
            for (int32 Z = 0; Z <= StepsZ; ++Z)
            {
                if (!DrawZAxis && Z != 0) continue;

                FVector CellMin = BoundingBox.Min + FVector(X * StepSize, Y * StepSize, Z * StepSize);
                FVector CellMax = CellMin + FVector(StepSize, StepSize, StepSize);

                float CellDensityCount = 0.0f;
                for (const FHeatmapData& Data : HeatmapArray)
                {
                    if (Data.X >= CellMin.X && Data.X < CellMax.X &&
                        Data.Y >= CellMin.Y && Data.Y < CellMax.Y &&
                        (DrawZAxis ? (Data.Z >= CellMin.Z && Data.Z < CellMax.Z) : true))
                    {
                        // このデータ点はこのセルの範囲内
                        // Data.Densityが記録回数なら+1、値がもうあるならそれを加算
                        CellDensityCount += Data.Density; 
                    }
                }

                CellDensities[GetCellIndex(X,Y,Z)] = CellDensityCount;
            }
        }
    }

    // Min/Max再計算
    float LocalMinDensity = FLT_MAX;
    float LocalMaxDensity = 0.0f;
    for (float CellVal : CellDensities)
    {
        if (CellVal < LocalMinDensity && CellVal > 0.0f)
        {
            LocalMinDensity = CellVal;
        }
        if (CellVal > LocalMaxDensity)
        {
            LocalMaxDensity = CellVal;
        }
    }

    // Min/Maxを反映
    MinDensityValue = LocalMinDensity == FLT_MAX ? 0.0f : LocalMinDensity;
    MaxDensityValue = LocalMaxDensity;

    // 色付けとDrawPositions生成
    float LogMin = FMath::Loge(MinDensityValue + 1.0f);
    float LogMax = FMath::Loge(MaxDensityValue + 1.0f);
    float Denominator = LogMax - LogMin;
    if (Denominator < KINDA_SMALL_NUMBER)
    {
        Denominator = KINDA_SMALL_NUMBER; 
    }

    for (int32 X = 0; X <= StepsX; ++X)
    {
        for (int32 Y = 0; Y <= StepsY; ++Y)
        {
            for (int32 Z = 0; Z <= StepsZ; ++Z)
            {
                if (!DrawZAxis && Z != 0) continue;

                float CellVal = CellDensities[GetCellIndex(X,Y,Z)];

                // 正規化
                float LogD = FMath::Loge(CellVal + 1.0f);
                float NormalizedDensity = (LogD - LogMin) / Denominator;
                NormalizedDensity = FMath::Clamp(NormalizedDensity * ColorScaleFactor, 0.0f, 1.0f);
            	if (NormalizedDensity < DrawMinDensity) continue;
            	
                FVector Position = BoundingBox.Min + FVector(X * StepSize, Y * StepSize, Z * StepSize);
                Position.Z += DrawZOffset;
                FColor Color = FColor::MakeRedToGreenColorFromScalar(NormalizedDensity);

                DrawPositions.Add({Position, Color});
            }
        }
    }
}
#undef LOCTEXT_NAMESPACE
