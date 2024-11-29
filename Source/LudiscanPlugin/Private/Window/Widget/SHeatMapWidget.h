#pragma once
#include "EditorModeManager.h"
#include "Client/LudiscanClient.h"
#include "HeatMap/HeatMapEdMode.h"
#include "Widgets/Input/SSlider.h"

class SHeatMapWidget: public SCompoundWidget
{
private:
	FPlaySessionResponseDto SelectedSession = FPlaySessionResponseDto();
	FString HostName;
	LudiscanClient Client = LudiscanClient();
	TSharedPtr<FHeatMapEdMode> EdMode;
public:
	SLATE_BEGIN_ARGS(SHeatMapWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args)
	{
		ActivateHeatmap();
		if (FHeatMapEdMode* CustomGizmoMode = static_cast<FHeatMapEdMode*>(GLevelEditorModeTools().GetActiveMode(FHeatMapEdMode::EM_HeatMapEdMode)))
		{
			EdMode = MakeShareable(CustomGizmoMode);
		}
		ChildSlot
		.Padding(5)
		[
			SNew(SVerticalBox)

			// 色強調度のスライダー
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Color Scale Factor"))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(0.8f)
				[
					SNew(SSlider)
					.Value(this, &SHeatMapWidget::GetColorScaleFactor)
					.OnValueChanged(this, &SHeatMapWidget::OnColorScaleFactorChanged)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.2f)
				[
					SNew(STextBlock)
					.Text(this, &SHeatMapWidget::GetColorScaleFactorText)
				]
			]

			// Z軸表示のチェックボックス
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SCheckBox)
				.IsChecked(this, &SHeatMapWidget::GetZAxisCheckState)
				.OnCheckStateChanged(this, &SHeatMapWidget::OnZAxisCheckStateChanged)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Draw Z-Axis"))
				]
			]
		];
	}
	
	
	void Reload(const FString& Name, FPlaySessionResponseDto Session)
	{
		SelectedSession = Session;
		HostName = Name;
		ActivateHeatmap();
		LoadHeatMap();
	}

	void Unload()
	{
		DeactivateHeatMap();
	}
private:
	FText GetColorScaleFactorText() const
	{
		return FText::FromString(FString::SanitizeFloat(GetColorScaleFactor()));
	}
	float GetColorScaleFactor() const
	{
		return EdMode->GetColorScaleFactor(); // EdMode から現在の値を取得
	}

	void OnColorScaleFactorChanged(float NewValue)
	{
		EdMode->SetColorScaleFactor(NewValue); // EdMode に新しい値を設定
		EdMode->RefreshDrawPositions(); // 描画位置を更新
	}

	ECheckBoxState GetZAxisCheckState() const
	{
		return EdMode->IsDrawZAxis() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void OnZAxisCheckStateChanged(ECheckBoxState NewState)
	{
		EdMode->SetDrawZAxis(NewState == ECheckBoxState::Checked);
		LoadHeatMap(EdMode->IsDrawZAxis());
	}

	void LoadHeatMap(bool zVisualize = false)
	{
		Client.SetConfig(HostName);
		Client.GetHeatMap(
			SelectedSession.ProjectId,
			SelectedSession.SessionId,
			[this](TArray<FPlaySessionHeatmapResponseDto> HeatmapList)
			{
				// for (const FPlaySessionHeatmapResponseDto& HeatmapData : HeatmapList)
				// {
					// UE_LOG(LogTemp, Log, TEXT("X: %f"), HeatmapData.X);
					// UE_LOG(LogTemp, Log, TEXT("Y: %f"), HeatmapData.Y);
					// UE_LOG(LogTemp, Log, TEXT("Z: %f"), HeatmapData.Z);
					// UE_LOG(LogTemp, Log, TEXT("Density: %f"), HeatmapData.Density);
				// }
				EdMode->SetHeatmapData(HeatmapList);
				EdMode->RefreshDrawPositions();
			},
			[this](FString Message)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get heatmap data: %s"), *Message);
				FText DialogText = FText::FromString(Message);
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			},
			300,
			zVisualize
		);
	}

	void ActivateHeatmap()
	{
		// Gizmoの表示モードが有効かどうかを確認
		if (!GLevelEditorModeTools().IsModeActive(FHeatMapEdMode::EM_HeatMapEdMode))
		{
			// モードが無効な場合は有効にする
			GLevelEditorModeTools().ActivateMode(FHeatMapEdMode::EM_HeatMapEdMode);
			UE_LOG(LogTemp, Warning, TEXT("Custom Gizmo Mode Activated"));
			// モードが有効な場合は無効にする
			// GLevelEditorModeTools().DeactivateMode(FHeatMapEdMode::EM_HeatMapEdMode);
			// UE_LOG(LogTemp, Warning, TEXT("Custom Gizmo Mode Deactivated"));
		}
	}

	void DeactivateHeatMap()
	{
		if (GLevelEditorModeTools().IsModeActive(FHeatMapEdMode::EM_HeatMapEdMode))
		{
			// モードが有効な場合は無効にする
			GLevelEditorModeTools().DeactivateMode(FHeatMapEdMode::EM_HeatMapEdMode);
			UE_LOG(LogTemp, Warning, TEXT("Custom Gizmo Mode Deactivated"));
		}
	}
};
