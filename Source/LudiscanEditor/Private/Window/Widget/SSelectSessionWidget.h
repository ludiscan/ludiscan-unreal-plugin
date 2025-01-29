#pragma once
#include "Client/FPlaySession.h"
#include "Client/LudiscanClient.h"
#include "Component/SCalcSessionsField.h"
#include "Component/SFiltersList.h"

DECLARE_DELEGATE_OneParam(FOnSessionSelected, TSharedPtr<FPlaySession>);
DECLARE_DELEGATE(FOnAllSessionSelected);

class LUDISCANEDITOR_API SSelectSessionWidget: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSelectSessionWidget) {}
	SLATE_EVENT(FOnSessionSelected, OnSessionSelected)
	SLATE_EVENT(FOnAllSessionSelected, OnAllSessionSelected)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args)
	{
		
        OnSessionSelected = Args._OnSessionSelected;
		OnAllSessionSelected = Args._OnAllSessionSelected;
		TArray<FString> Columns = { "SessionId", "ProjectId", "Name", "DeviceId", "Platform", "AppVersion", "StartTime", "EndTime" };
        ChildSlot
        [
        	SNew(SVerticalBox)
			+ SVerticalBox::Slot()
	        .FillHeight(1)
	        [
	            SNew(SVerticalBox)
        		+ SVerticalBox::Slot()
			    .AutoHeight()
			    .Padding(FMargin(10.0f, 5.0f))
			    [
			        SNew(SBorder)
			        .BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f)) // 背景色
			        .Padding(FMargin(5.0f))
			        [
			            SNew(SHorizontalBox)
			            + SHorizontalBox::Slot()
			            .AutoWidth()
			            .Padding(2.0f)
			            [
			                SNew(STextBlock)
			                .Text_Lambda([this]() -> FText {
		                		if (SelectedProject.Id != FProject().Id) {
			                        return FText::FromString(FString::FromInt(SelectedProject.Id));
			                    }
			                    return FText::FromString("ID: -");
			                })
			                .TextStyle(FAppStyle::Get(), "NormalText")
			                .MinDesiredWidth(50)
			            ]
			            + SHorizontalBox::Slot()
			            .AutoWidth()
			            .Padding(2.0f)
			            [
			                SNew(STextBlock)
			                .Text_Lambda([this]() -> FText {
			                    if (SelectedProject.Id != FProject().Id) {
			                        return FText::FromString(SelectedProject.Name);
			                    }
			                    return FText::FromString("No Name");
			                })
			                .TextStyle(FAppStyle::Get(), "NormalText")
			                .MinDesiredWidth(200)
			            ]
			            + SHorizontalBox::Slot()
			            .AutoWidth()
			            .Padding(2.0f)
			            [
			                SNew(STextBlock)
			                .Text_Lambda([this]() -> FText {
		                		if (SelectedProject.Id != FProject().Id) {
			                        return FText::FromString(SelectedProject.Description);
			                    }
			                    return FText::FromString("No Description");
			                })
			                .TextStyle(FAppStyle::Get(), "NormalText")
			                .MinDesiredWidth(180)
			            ]
			            + SHorizontalBox::Slot()
			            .AutoWidth()
			            .Padding(2.0f)
			            [
			                SNew(STextBlock)
			                .Text_Lambda([this]() -> FText {
		                		if (SelectedProject.Id != FProject().Id) {
			                        return FText::FromString(FormatTime(SelectedProject.CreatedAt));
			                    }
			                    return FText::FromString("N/A");
			                })
			                .TextStyle(FAppStyle::Get(), "NormalText")
			                .MinDesiredWidth(200)
			            ]
			        ]
			    ]

	            + SVerticalBox::Slot()
		        .AutoHeight()
		        [
			        SNew(SExpandableArea)
			        .AreaTitle(FText::FromString("Load Session"))
			        .BodyContent()
			        [
				        SNew(SVerticalBox)
				  //       + SVerticalBox::Slot()
      //   				.Padding(FMargin(10.0f, 5.0f)) // 左右: 10, 上下: 5
						// .HAlign(HAlign_Fill)           // 横幅いっぱいに
						// .VAlign(VAlign_Center)
			   //          .AutoHeight()
			   //          [
			   //              SNew(SHorizontalBox)
			   //              + SHorizontalBox::Slot()
			   //              .FillWidth(1.0f)
				  //           .Padding(5.0f, 0)
			   //              [
			   //                  SAssignNew(FilterTextBox, SEditableTextBox)
			   //                  .OnTextChanged(this, &SSelectSessionWidget::OnFilterTextChanged)
			   //                  .HintText(FText::FromString("Filter Sessions..."))
			   //              ]
			   //              + SHorizontalBox::Slot()
			   //              .AutoWidth()
			   //              [
			   //                  SNew(SButton)
			   //                  .Text(FText::FromString("MyDeviceSession"))
			   //                  .OnClicked(this, &SSelectSessionWidget::OnSortByDeviceId)
			   //              ]
			   //          ]
				        
			            + SVerticalBox::Slot()
			            .FillHeight(1.0f)
        				.Padding(FMargin(10.0f, 5.0f)) // 左右: 10, 上下: 5
						.HAlign(HAlign_Fill)           // 横幅いっぱいに
						.VAlign(VAlign_Center)
			            [
			                SNew(SScrollBox)
				            .Orientation(Orient_Vertical) // 縦方向のスクロールを有効にする
				            + SScrollBox::Slot()
				            [
	            				SNew(SScrollBox)
								.Orientation(Orient_Horizontal) // 横方向のスクロールを有効にする
								+ SScrollBox::Slot()
								[
									SAssignNew(SessionListView, SFiltersList<TSharedPtr<FPlaySession>>)
									.ItemHeight(40)
									.ColumnsSource(&Columns)
									.ListItemsSource(&SessionItems)
									.FilterHintText(FText::FromString("Filter Sessions..."))
									.SelectionMode(ESelectionMode::Type::SingleToggle)
									.OnSelectionChanged(this, &SSelectSessionWidget::OnSelectionChanged)
									.OnGetColumnString_Lambda([this](const TSharedPtr<FPlaySession>& InSession, const FString& Column) -> FString
									{
										if (Column == "SessionId")
										{
											return FString::FromInt(InSession->SessionId);
										}
										if (Column == "ProjectId")
										{
											return FString::FromInt(InSession->ProjectId);
										}
										if (Column == "Name")
										{
											return InSession->Name;
										}
										if (Column == "DeviceId")
										{
											return InSession->DeviceId;
										}
										if (Column == "Platform")
										{
											return InSession->Platform;
										}
										if (Column == "AppVersion")
										{
											return InSession->AppVersion;
										}
										if (Column == "StartTime")
										{
											return FormatTime(InSession->StartTime);
										}
										if (Column == "EndTime")
										{
											return FormatTime(InSession->EndTime);
										}
										return "";
									})
									.OnGetColumnFixedWidth_Lambda([this](const FString& Column) -> float
									{
										if (Column == "SessionId")
										{
											return 50;
										}
										if (Column == "ProjectId")
										{
											return 50;
										}
										if (Column == "Name")
										{
											return 200;
										}
										if (Column == "DeviceId")
										{
											return 200;
										}
										if (Column == "Platform")
										{
											return 100;
										}
										if (Column == "AppVersion")
										{
											return 100;
										}
										if (Column == "StartTime")
										{
											return 200;
										}
										if (Column == "EndTime")
										{
											return 200;
										}
										return 0;
									})
								]
				            ]
			            ]

			        	+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(5.0f)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.0f)
							[
								SNew(SButton)
								.Text(FText::FromString("Select All"))
								.OnClicked(this, &SSelectSessionWidget::OnSelectAllButtonClicked)
							]
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
						]
			        ]
		        ]

		        // 
	        	+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5.0f)
		        [
		        	SAssignNew(SCalcSessionsFieldRef, SCalcSessionsField)
		        ]
	        ]
	    ];
	}

	void Reload(const FString& HostName, const FProject& Project)
	{
		SelectedProject = Project;
		LoadSessions(HostName);
	}

	virtual ~SSelectSessionWidget() override
	{
		if (OnSessionSelected.IsBound())
		{
			OnSessionSelected.Unbind();
		}
		if (OnAllSessionSelected.IsBound())
		{
			OnAllSessionSelected.Unbind();
		}
	}
private:
	FProject SelectedProject = FProject();
	TArray<TSharedPtr<FPlaySession>> SessionItems;
	TSharedPtr<SFiltersList<TSharedPtr<FPlaySession>>> SessionListView;
	LudiscanAPI::LudiscanClient Client = LudiscanAPI::LudiscanClient();
	FOnSessionSelected OnSessionSelected;
	FOnAllSessionSelected OnAllSessionSelected;

	TSharedPtr<SCalcSessionsField> SCalcSessionsFieldRef;

	FReply OnSelectAllButtonClicked()
	{
		if (OnAllSessionSelected.IsBound())
		{
			auto _ = OnAllSessionSelected.ExecuteIfBound();
		}
		return FReply::Handled();
	}

	FReply OnSortByDeviceId()
    {
	    const FString MyDeviceId = FPlatformProcess::ComputerName();
    	// OnFilterTextChanged(FText::FromString(MyDeviceId));
    	return FReply::Handled();
    }

	void LoadSessions(FString HostName)
	{
		SessionItems.Empty();
		Client.SetConfig(HostName);
		Client.GetSessions(
			SelectedProject.Id,
			[this](TArray<FPlaySession> Sessions) {
				SessionItems.Reset();
				for (const FPlaySession& Session : Sessions)
				{
					SessionItems.Add(MakeShared<FPlaySession>(Session));
				}
				if (SessionItems.Num() == 0)
				{
					FText DialogText = FText::FromString("No sessions found.");
					FMessageDialog::Open(EAppMsgType::Ok, DialogText);
				}
				SessionListView->RefreshListItems();
			},
			[](FString ErrorMessage) {
				UE_LOG(LogTemp, Error, TEXT("Failed to load sessions: %s"), *ErrorMessage);
				FText DialogText = FText::FromString(ErrorMessage);
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			}
		);
	}

	void OnSelectionChanged(TSharedPtr<FPlaySession> Item, ESelectInfo::Type SelectInfo)
	{
		if (Item != nullptr && Item.IsValid())
		{
			auto _ = OnSessionSelected.ExecuteIfBound(Item);
		}
	}
	

	FString FormatTime(const FDateTime& TimeString)
	{
		if (TimeString == FDateTime())
		{
			return "N/A";
		}
		FDateTime ParsedTime;
		return TimeString.ToString(TEXT("%Y/%m/%d %H:%M:%S"));
	}
};
