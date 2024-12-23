#pragma once
#include "Client/FPlaySession.h"
#include "Client/LudiscanClient.h"
#include "Component/SCalcSessionsField.h"

DECLARE_DELEGATE_OneParam(FOnSessionSelected, TSharedPtr<FPlaySession>);
DECLARE_DELEGATE(FOnAllSessionSelected);

class SSelectSessionWidget: public SCompoundWidget
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
				        + SVerticalBox::Slot()
        				.Padding(FMargin(10.0f, 5.0f)) // 左右: 10, 上下: 5
						.HAlign(HAlign_Fill)           // 横幅いっぱいに
						.VAlign(VAlign_Center)
			            .AutoHeight()
			            [
			                SNew(SHorizontalBox)
			                + SHorizontalBox::Slot()
			                .FillWidth(1.0f)
				            .Padding(5.0f, 0)
			                [
			                    SAssignNew(FilterTextBox, SEditableTextBox)
			                    .OnTextChanged(this, &SSelectSessionWidget::OnFilterTextChanged)
			                    .HintText(FText::FromString("Filter Sessions..."))
			                ]
			                + SHorizontalBox::Slot()
			                .AutoWidth()
			                [
			                    SNew(SButton)
			                    .Text(FText::FromString("MyDeviceSession"))
			                    .OnClicked(this, &SSelectSessionWidget::OnSortByDeviceId)
			                ]
			            ]
				        
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
									SAssignNew(SessionListView, SListView<TSharedPtr<FPlaySession>>)
									.ItemHeight(40)
									.ListItemsSource(&FilteredSessionItems)
									.OnGenerateRow(this, &SSelectSessionWidget::OnGenerateSessionRow)
									.SelectionMode(ESelectionMode::Type::SingleToggle)
									.OnSelectionChanged(this, &SSelectSessionWidget::OnSelectionChanged)
									.HeaderRow(
										SNew(SHeaderRow)
										+ SHeaderRow::Column("ID")
										.DefaultLabel(FText::FromString("ID"))
										.OnSort(this, &SSelectSessionWidget::OnSortByID)
										.SortMode_Lambda([this]() { return GetSortMode("ID"); })
										.FixedWidth(50)
										.HAlignCell(HAlign_Center) // 中央揃え
										.VAlignCell(VAlign_Center)

										+ SHeaderRow::Column("Name")
										.DefaultLabel(FText::FromString("Session Name"))
										.OnSort(this, &SSelectSessionWidget::OnSortByName)
										.SortMode_Lambda([this]() { return GetSortMode("Name"); })
										.FixedWidth(200)
										.HAlignCell(HAlign_Center) // 中央揃え
										.VAlignCell(VAlign_Center)

										+ SHeaderRow::Column("Platform")
										.DefaultLabel(FText::FromString("Platform"))
										.OnSort(this, &SSelectSessionWidget::OnSortByPlatform)
										.SortMode_Lambda([this]() { return GetSortMode("Platform"); })
										.FixedWidth(100)
										.HAlignCell(HAlign_Center) // 中央揃え
										.VAlignCell(VAlign_Center)

										+ SHeaderRow::Column("Device ID")
										.DefaultLabel(FText::FromString("Device ID"))
										.OnSort(this, &SSelectSessionWidget::OnSortByDeviceId)
										.SortMode_Lambda([this]() { return GetSortMode("Device ID"); })
										.FixedWidth(150)
										.HAlignCell(HAlign_Center) // 中央揃え
										.VAlignCell(VAlign_Center)

										+ SHeaderRow::Column("Time")
										.DefaultLabel(FText::FromString("End/Start Time"))
										.OnSort(this, &SSelectSessionWidget::OnSortByTime)
										.SortMode_Lambda([this]() { return GetSortMode("Time"); })
										.FixedWidth(200)
										.HAlignCell(HAlign_Center) // 中央揃え
										.VAlignCell(VAlign_Center)
									)	
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
	TArray<TSharedPtr<FPlaySession>> FilteredSessionItems;
	TSharedPtr<SListView<TSharedPtr<FPlaySession>>> SessionListView;
	TSharedPtr<SEditableTextBox> FilterTextBox;
	LudiscanClient Client = LudiscanClient();
	FOnSessionSelected OnSessionSelected;
	FOnAllSessionSelected OnAllSessionSelected;
	// ソート状態を保持する変数
    FName CurrentSortColumn = "ID";
    EColumnSortMode::Type CurrentSortMode = EColumnSortMode::Ascending;

	TSharedPtr<SCalcSessionsField> SCalcSessionsFieldRef;


	// ソートモード取得
	EColumnSortMode::Type GetSortMode(const FName& ColumnId) const
	{
		if (CurrentSortColumn == ColumnId)
		{
			return CurrentSortMode;
		}
		return EColumnSortMode::None;
	}

	FReply OnSelectAllButtonClicked()
	{
		if (OnAllSessionSelected.IsBound())
		{
			auto _ = OnAllSessionSelected.ExecuteIfBound();
		}
		return FReply::Handled();
	}

    // ソート列とモードの切り替え
    void SetSortMode(FName ColumnId)
    {
        if (CurrentSortColumn == ColumnId)
        {
            CurrentSortMode = (CurrentSortMode == EColumnSortMode::Ascending) ? EColumnSortMode::Descending : EColumnSortMode::Ascending;
        }
        else
        {
            CurrentSortColumn = ColumnId;
            CurrentSortMode = EColumnSortMode::Ascending;
        }
    }

	void OnSortByID(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type NewSortMode)
	{
		SetSortMode("ID");
		FilteredSessionItems.Sort([this](const TSharedPtr<FPlaySession>& A, const TSharedPtr<FPlaySession>& B) {
			return CurrentSortMode == EColumnSortMode::Ascending ? (A->SessionId < B->SessionId) : (A->SessionId > B->SessionId);
		});
		SessionListView->RequestListRefresh();
	}

    void OnSortByName(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type NewSortMode)
    {
        SetSortMode("Name");
        FilteredSessionItems.Sort([this](const TSharedPtr<FPlaySession>& A, const TSharedPtr<FPlaySession>& B) {
            return CurrentSortMode == EColumnSortMode::Ascending ? (A->Name < B->Name) : (A->Name > B->Name);
        });
        SessionListView->RequestListRefresh();
    }

    void OnSortByPlatform(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type NewSortMode)
    {
        SetSortMode("Platform");
        FilteredSessionItems.Sort([this](const TSharedPtr<FPlaySession>& A, const TSharedPtr<FPlaySession>& B) {
            return CurrentSortMode == EColumnSortMode::Ascending ? (A->Platform < B->Platform) : (A->Platform > B->Platform);
        });
        SessionListView->RequestListRefresh();
    }

    void OnSortByDeviceId(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type NewSortMode)
    {
        SetSortMode("Device ID");
        FilteredSessionItems.Sort([this](const TSharedPtr<FPlaySession>& A, const TSharedPtr<FPlaySession>& B) {
            return CurrentSortMode == EColumnSortMode::Ascending ? (A->DeviceId < B->DeviceId) : (A->DeviceId > B->DeviceId);
        });
        SessionListView->RequestListRefresh();
    }

	TSharedRef<ITableRow> OnGenerateSessionRow(TSharedPtr<FPlaySession> InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		FString DisplayTime = FormatTime(InItem->EndTime.IsEmpty() ? InItem->StartTime : InItem->EndTime);

		return SNew(STableRow<TSharedPtr<FPlaySession>>, OwnerTable)
		.Style(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"))
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::FromInt(InItem->SessionId)))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.MinDesiredWidth(50)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(InItem->Name))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.MinDesiredWidth(200) // ヘッダーと同じ幅にする
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(InItem->Platform))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.MinDesiredWidth(100)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(InItem->DeviceId))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.MinDesiredWidth(150)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(DisplayTime))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.MinDesiredWidth(200)
				]
			]
		];
	}

	void OnFilterTextChanged(const FText& NewText)
	{
		const FString FilterText = NewText.ToString();
		FilteredSessionItems.Empty();
		if (!FilterText.IsEmpty())
		{
			TArray<FString> FilterTexts;
			FilterText.ParseIntoArray(FilterTexts, TEXT(","), true);
			if (FilterTexts.Num() != 0)
			{
				for (const FString& Text : FilterTexts)
				{
					if (Text.IsEmpty())
					{
						continue;
					}
					TArray<FString> KeyValue;
					Text.ParseIntoArray(KeyValue, TEXT(":"), true);

					for (TSharedPtr<FPlaySession> PlaySession : SessionItems)
					{
						if (KeyValue.Num() == 2)
						{
							// find metadata
							if (PlaySession->MetaData.Num() > 0)
							{
								for (const TPair<FString, FString>& Pair : PlaySession->MetaData)
								{
									if (Pair.Key.ToLower() == KeyValue[0].ToLower() && Pair.Value == KeyValue[1].ToLower())
									{
										FilteredSessionItems.Add(PlaySession);
									}
								}
							}
						} else
						{
							FilteredSessionItems = SessionItems.FilterByPredicate([&](const TSharedPtr<FPlaySession>& Session) {
								return FString::FromInt(Session->SessionId).Contains(Text) ||
									Session->Name.Contains(Text) ||
									Session->DeviceId.Contains(Text) ||
									Session->Platform.Contains(Text) ||
									Session->AppVersion.Contains(Text) ||
									Session->MetaData.Contains(Text);
							});
						}
					} 
				}
			} else
			{
				FilteredSessionItems = SessionItems;
			}
		}
		else
		{
			FilteredSessionItems = SessionItems;
		}
		if (SCalcSessionsFieldRef.IsValid())
		{
			SCalcSessionsFieldRef->SetDataListSources(FilteredSessionItems);
		}
		SessionListView->RequestListRefresh();
	}

	FReply OnSortByDeviceId()
    {
	    const FString MyDeviceId = FPlatformProcess::ComputerName();
    	OnFilterTextChanged(FText::FromString(MyDeviceId));
    	return FReply::Handled();
    }

	void LoadSessions(FString HostName)
	{
		SessionItems.Empty();
		OnFilterTextChanged(FilterTextBox->GetText());
		Client.SetConfig(HostName);
		Client.GetSessions(
			SelectedProject.Id,
			[this](TArray<FPlaySession> Sessions) {
				SessionItems.Reset();
				for (const FPlaySession& Session : Sessions)
				{
					SessionItems.Add(MakeShared<FPlaySession>(Session));
				}
				OnFilterTextChanged(FilterTextBox->GetText());
				if (SessionItems.Num() == 0)
				{
					FText DialogText = FText::FromString("No sessions found.");
					FMessageDialog::Open(EAppMsgType::Ok, DialogText);
				}
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
	void OnSortByTime(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type NewSortMode)
	{
		SetSortMode("Time");
		FilteredSessionItems.Sort([this](const TSharedPtr<FPlaySession>& A, const TSharedPtr<FPlaySession>& B) {
			FString TimeA = A->EndTime.IsEmpty() ? A->StartTime : A->EndTime;
			FString TimeB = B->EndTime.IsEmpty() ? B->StartTime : B->EndTime;
			return CurrentSortMode == EColumnSortMode::Ascending ? (TimeA < TimeB) : (TimeA > TimeB);
		});
		SessionListView->RequestListRefresh();
	}
	

	FString FormatTime(const FString& TimeString)
	{
		if (TimeString.IsEmpty())
		{
			return "N/A";
		}
		FDateTime ParsedTime;
		if (FDateTime::ParseIso8601(*TimeString, ParsedTime))
		{
			return ParsedTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
		}
		return TimeString; // フォーマットできない場合はそのまま返す
	}
};
