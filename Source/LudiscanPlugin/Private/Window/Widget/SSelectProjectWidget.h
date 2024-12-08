#pragma once
#include "Client/LudiscanClient.h"

DECLARE_DELEGATE_OneParam(FOnProjectSelected, TSharedPtr<FProject>);

class SSelectProjectWidget: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SSelectProjectWidget ) {}
	SLATE_EVENT(FOnProjectSelected, OnProjectSelected)
	SLATE_END_ARGS()
	void Construct(const FArguments& Args)
	{
		OnProjectSelected = Args._OnProjectSelected;
		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(FMargin(10.0f, 5.0f))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(5.0f, 0)
				[
					SAssignNew(FilterTextBox, SEditableTextBox)
					.OnTextChanged(this, &SSelectProjectWidget::OnFilterTextChanged)
					.HintText(FText::FromString("Filter Projects..."))
				]
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(FMargin(10.0f, 5.0f))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SBorder)
				[
					SNew(SScrollBox)
					.Orientation(Orient_Horizontal) // 横方向のスクロールを有効にする
					+ SScrollBox::Slot()
					[
						SAssignNew(ProjectListView, SListView<TSharedPtr<FProject>>)
						.ItemHeight(40)
						.ListItemsSource(&FilteredProjectItems)
						.OnGenerateRow(this, &SSelectProjectWidget::OnGenerateProjectRow)
						.SelectionMode(ESelectionMode::Type::SingleToggle)
						.OnSelectionChanged(this, &SSelectProjectWidget::OnSelectionChanged)
						.HeaderRow(
							SNew(SHeaderRow)
							+ SHeaderRow::Column("ID")
							.DefaultLabel(FText::FromString("ID"))
							.OnSort(this, &SSelectProjectWidget::OnSortByID)
							.SortMode_Lambda([this]() { return GetSortMode("ID"); })
							.FixedWidth(50)
							.HAlignCell(HAlign_Center)
							.VAlignCell(VAlign_Center)

							+ SHeaderRow::Column("Name")
							.DefaultLabel(FText::FromString("Project Name"))
							.OnSort(this, &SSelectProjectWidget::OnSortByName)
							.SortMode_Lambda([this]() { return GetSortMode("Name"); })
							.FixedWidth(200)
							.HAlignCell(HAlign_Center)
							.VAlignCell(VAlign_Center)

							+ SHeaderRow::Column("Description")
							.DefaultLabel(FText::FromString("Description"))
							.OnSort(this, &SSelectProjectWidget::OnSortByDescription)
							.SortMode_Lambda([this]() { return GetSortMode("Description"); })
							.FixedWidth(180)
							.HAlignCell(HAlign_Center)
							.VAlignCell(VAlign_Center)

							+ SHeaderRow::Column("Created At")
							.DefaultLabel(FText::FromString("Created At"))
							.OnSort(this, &SSelectProjectWidget::OnSortByCreatedAt)
							.SortMode_Lambda([this]() { return GetSortMode("Created At"); })
							.FixedWidth(200)
							.HAlignCell(HAlign_Center)
							.VAlignCell(VAlign_Center)
						)
					]
				]
			]
		];
	}

	void Reload(const FString& HostName)
	{
		LoadProjects(HostName);
	}

	virtual ~SSelectProjectWidget() override
	{
		if (OnProjectSelected.IsBound())
		{
			OnProjectSelected.Unbind();
		}
	}
private:
	TArray<TSharedPtr<FProject>> ProjectItems;
	TArray<TSharedPtr<FProject>> FilteredProjectItems;
	TSharedPtr<SListView<TSharedPtr<FProject>>> ProjectListView;
	TSharedPtr<SEditableTextBox> FilterTextBox;
	LudiscanClient Client = LudiscanClient();
	FOnProjectSelected OnProjectSelected;

	FName CurrentSortColumn = "ID";
	EColumnSortMode::Type CurrentSortMode = EColumnSortMode::Ascending;

	EColumnSortMode::Type GetSortMode(const FName& ColumnId) const
	{
		return (CurrentSortColumn == ColumnId) ? CurrentSortMode : EColumnSortMode::None;
	}

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

	void OnSortByID(const EColumnSortPriority::Type, const FName&, const EColumnSortMode::Type)
	{
		SetSortMode("ID");
		FilteredProjectItems.Sort([this](const TSharedPtr<FProject>& A, const TSharedPtr<FProject>& B) {
			return CurrentSortMode == EColumnSortMode::Ascending ? (A->Id < B->Id) : (A->Id > B->Id);
		});
		ProjectListView->RequestListRefresh();
	}

	void OnSortByName(const EColumnSortPriority::Type, const FName&, const EColumnSortMode::Type)
	{
		SetSortMode("Name");
		FilteredProjectItems.Sort([this](const TSharedPtr<FProject>& A, const TSharedPtr<FProject>& B) {
			return CurrentSortMode == EColumnSortMode::Ascending ? (A->Name < B->Name) : (A->Name > B->Name);
		});
		ProjectListView->RequestListRefresh();
	}

	void OnSortByDescription(const EColumnSortPriority::Type, const FName&, const EColumnSortMode::Type)
	{
		SetSortMode("Description");
		FilteredProjectItems.Sort([this](const TSharedPtr<FProject>& A, const TSharedPtr<FProject>& B) {
			return CurrentSortMode == EColumnSortMode::Ascending ? (A->Description < B->Description) : (A->Description > B->Description);
		});
		ProjectListView->RequestListRefresh();
	}

	void OnSortByCreatedAt(const EColumnSortPriority::Type, const FName&, const EColumnSortMode::Type)
	{
		SetSortMode("Created At");
		FilteredProjectItems.Sort([this](const TSharedPtr<FProject>& A, const TSharedPtr<FProject>& B) {
			return CurrentSortMode == EColumnSortMode::Ascending ? (A->CreatedAt < B->CreatedAt) : (A->CreatedAt > B->CreatedAt);
		});
		ProjectListView->RequestListRefresh();
	}

	TSharedRef<ITableRow> OnGenerateProjectRow(TSharedPtr<FProject> InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		return SNew(STableRow<TSharedPtr<FProject>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(InItem->Id)))
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
				.MinDesiredWidth(200)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InItem->Description))
				.TextStyle(FAppStyle::Get(), "NormalText")
				.MinDesiredWidth(180)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InItem->CreatedAt))
				.TextStyle(FAppStyle::Get(), "NormalText")
				.MinDesiredWidth(200)
			]
		];
	}

	void OnFilterTextChanged(const FText& NewText)
	{
		const FString FilterText = NewText.ToString();

		if (FilterText.IsEmpty())
		{
			FilteredProjectItems = ProjectItems;
		}
		else
		{
			FilteredProjectItems = ProjectItems.FilterByPredicate([&](const TSharedPtr<FProject>& Project) {
				return Project->Name.Contains(FilterText) ||
					   Project->Description.Contains(FilterText) ||
					   FString::FromInt(Project->Id).Contains(FilterText) ||
					   Project->CreatedAt.Contains(FilterText);
			});
		}
		ProjectListView->RequestListRefresh();
	}

	void LoadProjects(const FString& HostName)
	{
		Client.SetConfig(HostName);
		Client.GetProjects(
			[this](TArray<FProject> Projects) {
				ProjectItems.Reset();
				for (const FProject& Project : Projects)
				{
					ProjectItems.Add(MakeShared<FProject>(Project));
				}
				OnFilterTextChanged(FilterTextBox->GetText());
				if (ProjectItems.Num() == 0)
				{
					FText DialogText = FText::FromString("No projects found.");
					FMessageDialog::Open(EAppMsgType::Ok, DialogText);
				}
			},
			[](FString ErrorMessage) {
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage));
			}
		);
	}

	void OnSelectionChanged(TSharedPtr<FProject> Item, ESelectInfo::Type)
	{
		if (Item.IsValid())
		{
			OnProjectSelected.ExecuteIfBound(Item);
		}
	}
};