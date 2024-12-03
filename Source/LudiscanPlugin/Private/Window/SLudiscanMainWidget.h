#pragma once
#include "Client/Project.h"
#include "Widget/SHeatMapWidget.h"
#include "Widget/SSelectProjectWidget.h"
#include "Widget/SSelectSessionWidget.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

class SLudiscanMainWidget : public SCompoundWidget
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
			[
				SNew(SBorder)
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
					]
					+ SWidgetSwitcher::Slot()
					[
						SAssignNew(HeatmapWidget, SHeatMapWidget)
					]
				]
			]
		];
	}

private:
	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
	TSharedPtr<SSelectProjectWidget> ProjectWidget;
	TSharedPtr<SSelectSessionWidget> SessionWidget;
	TSharedPtr<SHeatMapWidget> HeatmapWidget;

	TSharedPtr<SEditableTextBox> HostnameInputBox;
	FString Hostname = "https://yuhi.tokyo";
	FProject SelectedProject = FProject();
	FPlaySessionResponseDto SelectedSession = FPlaySessionResponseDto();

	void OnProjectSelected(TSharedPtr<FProject> Project)
	{
		UE_LOG(LogTemp, Log, TEXT("Selected Project: %d"), Project->Id);
		SelectedProject = *Project;
		if (WidgetSwitcher.IsValid())
		{
			WidgetSwitcher->SetActiveWidget(SessionWidget.ToSharedRef());
			if (SessionWidget.IsValid())
			{
				SessionWidget->Reload(Hostname, *Project);
			}
		}
	}

	void OnSessionSelected(TSharedPtr<FPlaySessionResponseDto> Session)
	{
		UE_LOG(LogTemp, Log, TEXT("Selected Session: %d"), Session->SessionId);
		if (WidgetSwitcher.IsValid())
		{
			WidgetSwitcher->SetActiveWidget(HeatmapWidget.ToSharedRef());
			if (HeatmapWidget.IsValid())
			{
				SelectedSession = *Session;
				HeatmapWidget->Reload(Hostname, *Session);
			}
		}
	}

	FText GetHostName() const
	{
		const FString TempName = LudiscanClient::GetSaveApiHostName("https://yuhi.tokyo");
		if (TempName.IsEmpty())
		{
			return FText::FromString(Hostname); // デフォルト値を使用
		}
		return FText::FromString(TempName);
	}

	void OnHostNameCommitted(const FText& Text, ETextCommit::Type Arg)
	{
		Hostname = Text.ToString();
	}

	FReply OnReloadButtonClicked()
	{
		LudiscanClient::SetSaveApiHostName(Hostname);
		if (WidgetSwitcher.IsValid())
		{
			TSharedPtr<SWidget> ActiveWidget = WidgetSwitcher->GetActiveWidget();
			if (ActiveWidget == ProjectWidget)
			{
				HeatmapWidget->Unload();
				ProjectWidget->Reload(Hostname);
			}
			else if (ActiveWidget == SessionWidget)
			{
				HeatmapWidget->Unload();
				SessionWidget->Reload(Hostname, SelectedProject);
			}
			else if (ActiveWidget == HeatmapWidget)
			{
				HeatmapWidget->Reload(Hostname, SelectedSession);
			}
		}
		return FReply::Handled();
	}

	TSharedRef<SWidget> CreateInputHostNameSlot()
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.8f)
			[
				SAssignNew(HostnameInputBox, SEditableTextBox)
				.Text_Raw(this, &SLudiscanMainWidget::GetHostName)
				.OnTextCommitted_Raw(this, &SLudiscanMainWidget::OnHostNameCommitted)
				.HintText(FText::FromString("Enter API Hostname"))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString("Reload"))
				.OnClicked(this, &SLudiscanMainWidget::OnReloadButtonClicked)
			];
	}

	TSharedRef<SWidget> CreateSelectButtonsSlot()
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString("Project"))
				.OnClicked(this, &SLudiscanMainWidget::OnProjectButtonClicked)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(FText::FromString("Session"))
				.OnClicked(this, &SLudiscanMainWidget::OnSessionButtonClicked)
			];
			// + SHorizontalBox::Slot()
			// .AutoWidth()
			// [
			// 	SNew(SButton)
			// 	.Text(FText::FromString("Heatmap"))
			// 	.OnClicked(this, &SLudiscanMainWidget::OnHeatmapButtonClicked)
			// ];
	}

	FReply OnProjectButtonClicked()
	{
		WidgetSwitcher->SetActiveWidget(ProjectWidget.ToSharedRef());
		return FReply::Handled();
	}

	FReply OnSessionButtonClicked()
	{
		if (SelectedProject.Id == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Please select a project first."));
			FText DialogText = FText::FromString("Please select a project first.");
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			return FReply::Handled();
		}
		WidgetSwitcher->SetActiveWidget(SessionWidget.ToSharedRef());
		return FReply::Handled();
	}

	FReply OnHeatmapButtonClicked()
	{
		WidgetSwitcher->SetActiveWidget(HeatmapWidget.ToSharedRef());
		return FReply::Handled();
	}
};
