#pragma once
#include "Client/Project.h"
#include "Widget/SHeatMapWidget.h"
#include "Widget/SSelectProjectWidget.h"
#include "Widget/SSelectSessionWidget.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

class LUDISCANEDITOR_API SLudiscanMainWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLudiscanMainWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args)
	{
		ChildSlot
		[
			SNew(SVerticalBox)
			// 共通のヘッダー
			// hostname input
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateInputHostNameSlot()
			]
			// ボタン部分
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateSelectButtonsSlot()
			]

			// ウィジェット切り替え部分
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.MaxHeight(350)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)
				+ SScrollBox::Slot()
				.AutoSize()
				[
					SAssignNew(WidgetSwitcher, SWidgetSwitcher)
					+ SWidgetSwitcher::Slot()
					[
						SAssignNew(ProjectWidget, SSelectProjectWidget)
						.OnProjectSelected(this, &SLudiscanMainWidget::OnProjectSelected)
					]
					+ SWidgetSwitcher::Slot()
					[
						SAssignNew(SessionWidget, SSelectSessionWidget)
						.OnSessionSelected(this, &SLudiscanMainWidget::OnSessionSelected)
						.OnAllSessionSelected(this, &SLudiscanMainWidget::OnAllSessionSelected)
					]
					+ SWidgetSwitcher::Slot()
					[
						SAssignNew(HeatmapWidget, SHeatMapWidget)
					]
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateRequestEditor()
			]
		];
		Hostname = LudiscanAPI::LudiscanClient::GetSaveApiHostName("https://matuyuhi.com");
	}

	virtual ~SLudiscanMainWidget() override
	{
	    UE_LOG(LogTemp, Warning, TEXT("SLudiscanMainWidget Destructor Called"));

	    if (HostnameInputBox.IsValid())
	    {
	        // HostnameInputBox.Reset();
	        UE_LOG(LogTemp, Warning, TEXT("HostnameInputBox reset"));
	    }

	    if (HeatmapWidget.IsValid())
	    {
	    	HeatmapWidget->Unload();
	        // HeatmapWidget.Reset();
	        UE_LOG(LogTemp, Warning, TEXT("HeatmapWidget reset and unloaded"));
	    }
	}

private:
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
	TSharedPtr<SSelectProjectWidget> ProjectWidget;
	TSharedPtr<SSelectSessionWidget> SessionWidget;
	TSharedPtr<SHeatMapWidget> HeatmapWidget;

	TSharedPtr<SEditableTextBox> HostnameInputBox;
	FString Hostname = "";
	int StepSize = 200;
	bool ZVisible = false;
	FProject SelectedProject = FProject();
	FHeatMapTask SelectedTask = FHeatMapTask();
	TFunction<void()> LastHeatMapTaskPolling = []() {};
	
	LudiscanAPI::LudiscanClient Client = LudiscanAPI::LudiscanClient();

	void OnProjectSelected(TSharedPtr<FProject> Project);

	void OnSessionSelected(TSharedPtr<FPlaySession> InSession);

	void OnAllSessionSelected();

	FText GetHostName() const;

	void OnHostNameCommitted(const FText& Text, ETextCommit::Type Arg);

	void OnHostNameChanged(const FText& Text)
	{
		Hostname = Text.ToString();
	}

	FReply OnReloadButtonClicked();

	TSharedRef<SWidget> CreateInputHostNameSlot();

	void OnStepSizeCommitted(const FText& Text, ETextCommit::Type Arg)
	{
		StepSize = FCString::Atoi(*Text.ToString());
	}

	void OnStepSizeChanged(const FText& Text)
	{
		StepSize = FCString::Atoi(*Text.ToString());
	}

	void OnZAxisCheckStateChanged(ECheckBoxState CheckBoxState)
	{
		ZVisible = CheckBoxState == ECheckBoxState::Checked;
	}

	float GetStepSize() const
	{
		return StepSize;
	}

	void OnDrawStepSizeChanged(float X)
	{
		StepSize = FMath::RoundToInt(X);
	}

	FText GetDrawStepSizeText() const
	{
		return FText::AsNumber(StepSize);
	}
	// enter StepSize (100~1000) & zVisible (true/false)
	TSharedRef<SWidget> CreateRequestEditor();

	TSharedRef<SWidget> CreateSelectButtonsSlot();

	FReply OnProjectButtonClicked() const;

	FReply OnSessionButtonClicked() const;

	FReply OnHeatmapButtonClicked() const;

	void CreateSessionTaskAndActivateHeatmap(
		const FPlaySession& InSession = FPlaySession(),
		int InStepSize = 100,
		bool InZVisible = false
		);

	void CreateProjectTaskAndActivateHeatmap(
		const FProject& InProject = FProject(),
		int InStepSize = 100,
		bool InZVisible = false
		);
};
