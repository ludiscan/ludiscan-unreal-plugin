#pragma once
#include "EditorModeManager.h"
#include "Client/LudiscanClient.h"
#include "HeatMap/HeatMapEdMode.h"
#include "Widgets/Input/SSlider.h"

class SHeatMapWidget: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHeatMapWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args)
	{
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
		        	SessionInfoRow()
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
	FString HostName;
	LudiscanClient Client = LudiscanClient();
	TWeakObjectPtr<UHeatMapEdMode> EdMode;
	FTimerHandle TaskPollingTimerHandle;
	UWorld* World = GEditor->GetEditorWorldContext().World();

	TSharedPtr<SVerticalBox> VerticalBox;

	bool IsLoading = false;

	// ポーリング間隔（秒単位）
	const float TaskPollingInterval = 1.5f;

	void SetTask(const FHeatMapTask& NewTask)
	{
		SelectedTask = NewTask;
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
		Client.GetTask(
			SelectedTask,
			[this](const FHeatMapTask& Task) {
				SelectedTask = Task;
				SelectedTask.Log();
				if (SelectedTask.Status == FHeatMapTask::Completed)
				{
					IsLoading = false;
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
	
	TSharedRef<SBorder> SessionInfoRow()
	{
		if (IsLoading)
		{
			return SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f)) // 背景色
				.Padding(FMargin(5.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString("Loading..."))
					.TextStyle(FAppStyle::Get(), "NormalText")
				];
		}
		FPlaySession Session = SelectedTask.Session;
		return SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f)) // 背景色
        .Padding(FMargin(5.0f))
        [
            SNew(SVerticalBox)
            // セッション ID
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(Session.SessionId > 0
                        ? FString::Printf(TEXT("Session ID: %d"), Session.SessionId)
                        : FString(TEXT("No session selected")));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
            // プロジェクト ID
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(FString::Printf(TEXT("Project ID: %d"), Session.ProjectId));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
            // セッション名
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(!Session.Name.IsEmpty()
                        ? FString::Printf(TEXT("Name: %s"), *Session.Name)
                        : FString(TEXT("Name: Unknown")));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
            // デバイス ID
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(!Session.DeviceId.IsEmpty()
                        ? FString::Printf(TEXT("Device ID: %s"), *Session.DeviceId)
                        : FString(TEXT("Device ID: Unknown")));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
            // プラットフォーム
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(!Session.Platform.IsEmpty()
                        ? FString::Printf(TEXT("Platform: %s"), *Session.Platform)
                        : FString(TEXT("Platform: Unknown")));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
            // アプリバージョン
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(!Session.AppVersion.IsEmpty()
                        ? FString::Printf(TEXT("App Version: %s"), *Session.AppVersion)
                        : FString(TEXT("App Version: Unknown")));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
            // 開始時刻
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(!Session.StartTime.IsEmpty()
                        ? FString::Printf(TEXT("Start Time: %s"), *Session.StartTime)
                        : FString(TEXT("Start Time: Unknown")));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
            // 終了時刻
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(2.0f)
            [
                SNew(STextBlock)
                .Text_Lambda([this, Session]() -> FText {
                    return FText::FromString(!Session.EndTime.IsEmpty()
                        ? FString::Printf(TEXT("End Time: %s"), *Session.EndTime)
                        : FString(TEXT("End Time: Unknown")));
                })
                .TextStyle(FAppStyle::Get(), "NormalText")
            ]
	        + SVerticalBox::Slot()
	        .AutoHeight()
	        .Padding(2.0f)
	        [
	        	SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString("MetaData:"))
					.TextStyle(FAppStyle::Get(), "NormalText")
				]
				// MetaData の各キーと値を表示
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SVerticalBox)
					.Clipping(EWidgetClipping::ClipToBounds)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(2.0f)
					.HAlign(HAlign_Left)
					[
						SNew(STextBlock)
						.Text_Lambda([this, Session]() -> FText {
							FString MetaDataString;
							for (const auto& Pair : Session.MetaData)
							{
								MetaDataString += FString::Printf(TEXT("%s: %s\n"), *Pair.Key, *Pair.Value);
							}
							return FText::FromString(MetaDataString.IsEmpty()
								? FString(TEXT("No metadata available"))
								: MetaDataString);
						})
						.TextStyle(FAppStyle::Get(), "NormalText")
					]
				]
	        ]
        ];
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
