#include "SLudiscanMainWidget.h"

void SLudiscanMainWidget::OnProjectSelected(TSharedPtr<FProject> Project)
{
	UE_LOG(LogTemp, Log, TEXT("Selected Project: %d"), Project->Id);
	SelectedProject = *Project;
	LudiscanAPI::LudiscanClient::SetSaveProjectId(SelectedProject.Id);
	if (WidgetSwitcher.IsValid())
	{
		if (SessionWidget.IsValid())
		{
			WidgetSwitcher->SetActiveWidget(SessionWidget.ToSharedRef());
			SessionWidget->Reload(Hostname, *Project);
		}
	}
}

void SLudiscanMainWidget::OnSessionSelected(TSharedPtr<FPlaySession> InSession)
{
	LastHeatMapTaskPolling = [this, InSession]()
	{
		CreateSessionTaskAndActivateHeatmap(
			*InSession,
			StepSize,
			ZVisible);
	};
	LastHeatMapTaskPolling();
}

void SLudiscanMainWidget::OnAllSessionSelected()
{
	LastHeatMapTaskPolling = [this]()
	{
		CreateProjectTaskAndActivateHeatmap(
			SelectedProject,
			StepSize,
			ZVisible);
	};
	LastHeatMapTaskPolling();
}

FText SLudiscanMainWidget::GetHostName() const
{
	if (Hostname.IsEmpty())
	{
		const FString TempName = LudiscanAPI::LudiscanClient::GetSaveApiHostName("https://yuhi.tokyo");
		if (TempName.IsEmpty())
		{
			return FText::FromString(Hostname); // デフォルト値を使用
		}
		return FText::FromString(TempName);
	}
	return FText::FromString(Hostname);
}

void SLudiscanMainWidget::OnHostNameCommitted(const FText& Text, ETextCommit::Type Arg)
{
	Hostname = Text.ToString();
	LudiscanAPI::LudiscanClient::SetSaveApiHostName(Hostname);
	Client.SetConfig(Hostname);
}

FReply SLudiscanMainWidget::OnReloadButtonClicked()
{
	LudiscanAPI::LudiscanClient::SetSaveApiHostName(Hostname);
	Client.SetConfig(Hostname);
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
			if (SelectedProject.Id != 0)
			{
				HeatmapWidget->Unload();
				SessionWidget->Reload(Hostname, SelectedProject);
			}
		}
		else if (ActiveWidget == HeatmapWidget)
		{
			if (SelectedTask.TaskId != FHeatMapTask().TaskId)
			{
				HeatmapWidget->Unload();
				LastHeatMapTaskPolling();
			}
		}
	}
	return FReply::Handled();
}

TSharedRef<SWidget> SLudiscanMainWidget::CreateInputHostNameSlot()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.8f)
		[
			SAssignNew(HostnameInputBox, SEditableTextBox)
			.Text_Raw(this, &SLudiscanMainWidget::GetHostName)
			.OnTextChanged(this, &SLudiscanMainWidget::OnHostNameChanged)
			.OnTextCommitted(this, &SLudiscanMainWidget::OnHostNameCommitted)
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

TSharedRef<SWidget> SLudiscanMainWidget::CreateRequestEditor()
{
	return SNew(SExpandableArea)
		.AreaTitle(FText::FromString("Request Editor"))
		.BodyContent()
		[
			SNew(SVerticalBox)
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
					.Value(this, &SLudiscanMainWidget::GetStepSize)
					.OnValueChanged(this, &SLudiscanMainWidget::OnDrawStepSizeChanged)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.2f)
				[
					SNew(STextBlock)
					.Text(this, &SLudiscanMainWidget::GetDrawStepSizeText)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(5)
			[
				SNew(SCheckBox)
				.IsChecked(ZVisible)
				.OnCheckStateChanged(this, &SLudiscanMainWidget::OnZAxisCheckStateChanged)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Draw Z-Axis"))
				]
			]
		];
}

TSharedRef<SWidget> SLudiscanMainWidget::CreateSelectButtonsSlot()
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

FReply SLudiscanMainWidget::OnProjectButtonClicked() const
{
	WidgetSwitcher->SetActiveWidget(ProjectWidget.ToSharedRef());
	return FReply::Handled();
}

FReply SLudiscanMainWidget::OnSessionButtonClicked() const
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

FReply SLudiscanMainWidget::OnHeatmapButtonClicked() const
{
	WidgetSwitcher->SetActiveWidget(HeatmapWidget.ToSharedRef());
	return FReply::Handled();
}

void SLudiscanMainWidget::CreateSessionTaskAndActivateHeatmap(const FPlaySession& InSession, int InStepSize,
                                                              bool InZVisible)
{
	if (InSession.SessionId == FPlaySession().SessionId)
	{
			
	} else
	{
		Client.CreateSessionHeatMap(
			InSession.ProjectId,
			InSession.SessionId,
			[this](FHeatMapTask Task) {
				UE_LOG(LogTemp, Log, TEXT("Created session heatmap task: %d"), Task.TaskId);
				Task.Log();
				SelectedTask = Task;
				if (WidgetSwitcher.IsValid())
				{
					WidgetSwitcher->SetActiveWidget(HeatmapWidget.ToSharedRef());
					if (HeatmapWidget.IsValid())
					{
						HeatmapWidget->Reload(Hostname, Task);
					}
				}
			},
			[this](const FString& Message) {
				UE_LOG(LogTemp, Error, TEXT("Failed to create heatmap tasks: %s"), *Message);
				FText DialogText = FText::FromString(Message);
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			},
			InStepSize,
			InZVisible
		);
	}
        
}

void SLudiscanMainWidget::CreateProjectTaskAndActivateHeatmap(const FProject& InProject, int InStepSize,
                                                              bool InZVisible)
{
	if (InProject.Id == FProject().Id)
	{
		return;
	}
	Client.CreateProjectHeatMap(
		InProject.Id,
		[this](FHeatMapTask Task) {
			UE_LOG(LogTemp, Log, TEXT("Created project heatmap task: %d"), Task.TaskId);
			Task.Log();
			SelectedTask = Task;
			if (WidgetSwitcher.IsValid())
			{
				WidgetSwitcher->SetActiveWidget(HeatmapWidget.ToSharedRef());
				if (HeatmapWidget.IsValid())
				{
					HeatmapWidget->Reload(Hostname, Task);
				}
			}
		},
		[this](const FString& Message) {
			UE_LOG(LogTemp, Error, TEXT("Failed to create heatmap tasks: %s"), *Message);
			FText DialogText = FText::FromString(Message);
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
		},
		InStepSize,
		InZVisible
	);
}
