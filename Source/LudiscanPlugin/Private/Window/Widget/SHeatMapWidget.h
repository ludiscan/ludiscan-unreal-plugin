#pragma once
#include "EditorModeManager.h"
#include "Client/LudiscanClient.h"
#include "Component/SHeatMapDetail.h"
#include "HeatMap/HeatMapEdMode.h"
#include "Widgets/Input/SSlider.h"

class SHeatMapWidget: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHeatMapWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args)
	{
		IsLoading = true;
		if (SHeatMapDetailRef.IsValid())
		{
			SHeatMapDetailRef->SetIsLoading(true);
		} 
		ActivateHeatmap();
		if (UEdMode* CustomGizmoMode = GLevelEditorModeTools().GetActiveScriptableMode(UHeatMapEdMode::EM_HeatMapEdMode))
		{
			EdMode = Cast<UHeatMapEdMode>(CustomGizmoMode);
		}
		ChildSlot
		.Padding(5)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(300)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)
				+ SScrollBox::Slot()
				.AutoSize()
			
				[
					SAssignNew(VerticalBox, SVerticalBox)
					// 現在のセッション情報を表示
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					.Padding(5)
					[
						SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SAssignNew(SHeatMapDetailRef, SHeatMapDetail)
						]
					]
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
					
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(5)
					[
						SNew(STextBlock)
						.Text(FText::FromString("DrawStepSize"))
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
							.MinValue(50)
							.MaxValue(1000)
							.Value(this, &SHeatMapWidget::GetStepSize)
							.OnValueChanged(this, &SHeatMapWidget::OnDrawStepSizeChanged)
						]
						+ SHorizontalBox::Slot()
						.FillWidth(0.2f)
						[
							SNew(STextBlock)
							.Text(this, &SHeatMapWidget::GetDrawStepSizeText)
						]
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(5)
					[
						SNew(STextBlock)
						.Text(FText::FromString("DrawZOffset"))
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
							.MinValue(-2000)
							.MaxValue(2000)
							.Value(this, &SHeatMapWidget::GetDrawZOffset)
							.OnValueChanged(this, &SHeatMapWidget::OnDrawZOffsetChanged)
						]
						+ SHorizontalBox::Slot()
						.FillWidth(0.2f)
						[
							SNew(SEditableText)
							.Text(this, &SHeatMapWidget::GetDrawZOffsetText)
							.OnTextCommitted(this, &SHeatMapWidget::OnDrawZOffsetCommitted)
						]
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(5)
					[
						SNew(SCheckBox)
						.IsChecked(this, &SHeatMapWidget::GetDrawMinDensityCheckState)
						.OnCheckStateChanged(this, &SHeatMapWidget::OnDrawMinDensityCheckStateChanged)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Draw Min Density"))
						]
					]
				]
			]
		];
	}

	SHeatMapWidget(): IsLoading(false)
	{
	}

	void Reload(const FString& Name, const FHeatMapTask& NewTask)
	{
		HostName = Name;
		if (UEdMode* CustomGizmoMode = GLevelEditorModeTools().GetActiveScriptableMode(UHeatMapEdMode::EM_HeatMapEdMode))
		{
			EdMode = Cast<UHeatMapEdMode>(CustomGizmoMode);
		}
		SetTask(NewTask);
	}

	virtual ~SHeatMapWidget() override
	{
		Unload();
	}

	void Unload()
	{
		if (!IsEngineExitRequested())
		{
			DeactivateHeatMap();
		}
		// タイマーが動作している場合はクリア
		if (World && World->GetTimerManager().IsTimerActive(TaskPollingTimerHandle))
		{
			World->GetTimerManager().ClearTimer(TaskPollingTimerHandle);
		}
	}
private:
	FHeatMapTask SelectedTask = FHeatMapTask();
	TSharedPtr<SHeatMapDetail> SHeatMapDetailRef;
	FString HostName;
	LudiscanClient Client = LudiscanClient();
	TWeakObjectPtr<UHeatMapEdMode> EdMode;
	FTimerHandle TaskPollingTimerHandle;
	UWorld* World = GEditor->GetEditorWorldContext().World();

	TSharedPtr<SVerticalBox> VerticalBox;

	bool IsLoading;

	// ポーリング間隔（秒単位）
	const float TaskPollingInterval = 1.5f;

	void SetTask(const FHeatMapTask& NewTask)
	{
		if (NewTask.TaskId == FHeatMapTask().TaskId)
		{
			return;
		}
		SelectedTask = NewTask;
		if (SHeatMapDetailRef.IsValid())
		{
			SHeatMapDetailRef->SetSession(SelectedTask.Session);
			SHeatMapDetailRef->SetProject(SelectedTask.Project);
		}
		if (SHeatMapDetailRef.IsValid())
		{
			SHeatMapDetailRef->SetIsLoading(true);
		}
		if (World)
		{
			World->GetTimerManager().ClearTimer(TaskPollingTimerHandle);
		}

		// 最初の getTask 呼び出し
		if (World)
		{
			World->GetTimerManager().SetTimer(
				TaskPollingTimerHandle,
				FTimerDelegate::CreateSP(this, &SHeatMapWidget::PollGetTask),
				TaskPollingInterval,
				false
			);
		}
		Invalidate(EInvalidateWidgetReason::Paint);
	}

	void PollGetTask()
	{
		IsLoading = true;
		if (SHeatMapDetailRef.IsValid())
		{
			SHeatMapDetailRef->SetIsLoading(true);
		}
		Client.GetTask(
			SelectedTask,
			[this](const FHeatMapTask& Task) {
				SelectedTask = Task;
				SelectedTask.Log();
				if (SHeatMapDetailRef.IsValid())
				{
					SHeatMapDetailRef->SetSession(SelectedTask.Session);
					SHeatMapDetailRef->SetProject(SelectedTask.Project);
				}
				if (SelectedTask.Status == FHeatMapTask::Completed)
				{
					IsLoading = false;
					if (SHeatMapDetailRef.IsValid())
					{
						SHeatMapDetailRef->SetIsLoading(false);
					}
					
					// タイマーを停止
					if (World)
	                {
	                    World->GetTimerManager().ClearTimer(TaskPollingTimerHandle);
	                }
					if (UEdMode* CustomGizmoMode = GLevelEditorModeTools().GetActiveScriptableMode(UHeatMapEdMode::EM_HeatMapEdMode))
					{
						EdMode = Cast<UHeatMapEdMode>(CustomGizmoMode);
					}
					if (EdMode.IsValid())
					{
						ActivateHeatmap();
						EdMode->SetHeatmapData(SelectedTask.HeatMapDataArray);
						EdMode->RefreshDrawPositions();
					}
				} else if (SelectedTask.Status == FHeatMapTask::Failed)
				{
					IsLoading = false;
					if (SHeatMapDetailRef.IsValid())
					{
						SHeatMapDetailRef->SetIsLoading(false);
					}
					if (World)
					{
						World->GetTimerManager().ClearTimer(TaskPollingTimerHandle);
					}
					UE_LOG(LogTemp, Error, TEXT("Failed to get task: タスクの取得に失敗しました"));
					FText DialogText = FText::FromString("Failed to get task: タスクの取得に失敗しました");
					FMessageDialog::Open(EAppMsgType::Ok, DialogText);
				} else
				{
					IsLoading = true;
					if (SHeatMapDetailRef.IsValid())
					{
						SHeatMapDetailRef->SetIsLoading(true);
					}
					// タスクがまだ進行中の場合、次のポーリングをスケジュール
					if (World)
					{
						World->GetTimerManager().SetTimer(
							TaskPollingTimerHandle,
							FTimerDelegate::CreateSP(this, &SHeatMapWidget::PollGetTask),
							TaskPollingInterval,
							false
						);
					}
				}
				Invalidate(EInvalidateWidgetReason::Paint);
			},
			[this](const FString& Message) {
				if (SHeatMapDetailRef.IsValid())
				{
					SHeatMapDetailRef->SetIsLoading(false);
				}
				// タイマーを停止
				if (World)
				{
				   World->GetTimerManager().ClearTimer(TaskPollingTimerHandle);
				}
				UE_LOG(LogTemp, Error, TEXT("Failed to get task: %s"), *Message);
				FText DialogText = FText::FromString(Message);
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			}
		);
		Invalidate(EInvalidateWidgetReason::Paint);
	}
	
	FText GetColorScaleFactorText() const
	{
		return FText::FromString(FString::SanitizeFloat(GetColorScaleFactor()));
	}
	float GetColorScaleFactor() const
	{
		return EdMode->GetColorScaleFactor(); // EdMode から現在の値を取得
	}

	void OnColorScaleFactorChanged(float NewValue) const
	{
		EdMode->SetColorScaleFactor(NewValue); // EdMode に新しい値を設定
		EdMode->RefreshDrawPositions(); // 描画位置を更新
	}

	ECheckBoxState GetZAxisCheckState() const
	{
		return EdMode->IsDrawZAxis() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void OnZAxisCheckStateChanged(ECheckBoxState NewState) const
	{
		EdMode->SetDrawZAxis(NewState == ECheckBoxState::Checked);
		EdMode->RefreshDrawPositions();
	}

	static void ActivateHeatmap()
	{
		// Gizmoの表示モードが有効かどうかを確認
		if (GLevelEditorModeToolsIsValid() && !GLevelEditorModeTools().IsModeActive(UHeatMapEdMode::EM_HeatMapEdMode))
		{
			// モードが無効な場合は有効にする
			GLevelEditorModeTools().ActivateMode(UHeatMapEdMode::EM_HeatMapEdMode);
			UE_LOG(LogTemp, Warning, TEXT("Custom Gizmo Mode Activated"));
		}
	}

	void DeactivateHeatMap()
	{
		if (GLevelEditorModeToolsIsValid() && GLevelEditorModeTools().IsModeActive(UHeatMapEdMode::EM_HeatMapEdMode))
		{
			GLevelEditorModeTools().DeactivateMode(UHeatMapEdMode::EM_HeatMapEdMode);
			UE_LOG(LogTemp, Warning, TEXT("Custom Gizmo Mode Deactivated"));
		}
	}

	float GetStepSize() const
	{
		return EdMode->GetDrawStepSize();
	}

	void OnDrawStepSizeChanged(float X) const
	{
		EdMode->SetDrawStepSize(X);
		EdMode->RefreshDrawPositions();
	}
	
	FText GetDrawStepSizeText() const
	{
		return FText::AsNumber(EdMode->GetDrawStepSize());
	}

	float GetDrawZOffset() const
	{
		return EdMode->GetDrawZOffset();
	}

	void OnDrawZOffsetChanged(float X) const
	{
		EdMode->SetDrawZOffset(X);
		EdMode->RefreshDrawPositions();
	}

	FText GetDrawZOffsetText() const
	{
		return FText::AsNumber(EdMode->GetDrawZOffset());
	}
	
	void OnDrawZOffsetCommitted(const FText& Text, ETextCommit::Type Arg)
	{
		EdMode->SetDrawZOffset(FCString::Atoi(*Text.ToString()));
		EdMode->RefreshDrawPositions();
	}

	ECheckBoxState GetDrawMinDensityCheckState() const
	{
		return EdMode->IsDrawMinDensity() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	void OnDrawMinDensityCheckStateChanged(ECheckBoxState NewState) const
	{
		EdMode->SetDrawMinDensity(NewState == ECheckBoxState::Checked);
		EdMode->RefreshDrawPositions();
	}
};
