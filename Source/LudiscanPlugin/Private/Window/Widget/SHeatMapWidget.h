#pragma once
#include "EditorModeManager.h"
#include "Client/LudiscanClient.h"
#include "Component/SSessionDetail.h"
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
		if (SSessionDetailRef.IsValid())
		{
			SSessionDetailRef->SetIsLoading(true);
		} 
		ActivateHeatmap();
		if (UEdMode* CustomGizmoMode = GLevelEditorModeTools().GetActiveScriptableMode(UHeatMapEdMode::EM_HeatMapEdMode))
		{
			EdMode = Cast<UHeatMapEdMode>(CustomGizmoMode);
		}
		ChildSlot
		.Padding(5)
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
		        	SAssignNew(SSessionDetailRef, SSessionDetail)
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
		];
	}

	SHeatMapWidget()
	{
	}
	
	void Reload(const FString& Name, const FHeatMapTask& NewTask)
	{
		HostName = Name;
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
	TSharedPtr<SSessionDetail> SSessionDetailRef;
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
		SelectedTask = NewTask;
		if (SSessionDetailRef.IsValid())
		{
			SSessionDetailRef->SetSession(SelectedTask.Session);
		}
		if (SSessionDetailRef.IsValid())
		{
			SSessionDetailRef->SetIsLoading(true);
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
		if (SSessionDetailRef.IsValid())
		{
			SSessionDetailRef->SetIsLoading(true);
		}
		Client.GetTask(
			SelectedTask,
			[this](const FHeatMapTask& Task) {
				SelectedTask = Task;
				SelectedTask.Log();
				if (SSessionDetailRef.IsValid())
				{
					SSessionDetailRef->SetSession(SelectedTask.Session);
				}
				if (SelectedTask.Status == FHeatMapTask::Completed)
				{
					IsLoading = false;
					if (SSessionDetailRef.IsValid())
					{
						SSessionDetailRef->SetIsLoading(false);
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
					if (SSessionDetailRef.IsValid())
					{
						SSessionDetailRef->SetIsLoading(false);
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
					if (SSessionDetailRef.IsValid())
					{
						SSessionDetailRef->SetIsLoading(true);
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
				if (SSessionDetailRef.IsValid())
				{
					SSessionDetailRef->SetIsLoading(false);
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
};
