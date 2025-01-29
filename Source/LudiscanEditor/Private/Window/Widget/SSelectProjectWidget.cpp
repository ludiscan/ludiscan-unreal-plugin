#include "SSelectProjectWidget.h"

void SSelectProjectWidget::Construct(const FArguments& Args)
{
	OnProjectSelected = Args._OnProjectSelected;
	TArray<FString> Columns = { "ID", "Name", "Description", "Created At" };
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SScrollBox)
			.Orientation(Orient_Horizontal) // 横方向のスクロールを有効にする
			+ SScrollBox::Slot()
			[
				SAssignNew(ProjectListView, SFiltersList<TSharedPtr<FProject>>)
				.ItemHeight(40)
				.ColumnsSource(&Columns)
				.ListItemsSource(&ProjectItems)
				.FilterHintText(FText::FromString("Filter Projects..."))
				.SelectionMode(ESelectionMode::Type::SingleToggle)
				.OnSelectionChanged(this, &SSelectProjectWidget::OnSelectionChanged)
				.OnGetColumnString_Lambda([this](const TSharedPtr<FProject>& InProject, const FString& Column) -> FString
				{
					if (Column == "ID")
					{
						return FString::FromInt(InProject->Id);
					}
					if (Column == "Name")
					{
						return InProject->Name;
					}
					if (Column == "Description")
					{
						return InProject->Description;
					}
					if (Column == "Created At")
					{
						return InProject->CreatedAt.ToString();
					}
					return "";
				})
				.OnGetColumnFixedWidth_Lambda([this](const FString& Column) -> float
				{
					if (Column == "ID")
					{
						return 70;
					}
					if (Column == "Name")
					{
						return 200;
					}
					if (Column == "Description")
					{
						return 120;
					}
					if (Column == "Created At")
					{
						return 200;
					}
					return 0;
				})
			]
		]
	];
}

void SSelectProjectWidget::LoadProjects(const FString& HostName)
{
	Client.SetConfig(HostName);
	Client.GetProjects(
		[this](TArray<FProject> Projects) {
			ProjectItems.Reset();
			for (const FProject& Project : Projects)
			{
				ProjectItems.Add(MakeShared<FProject>(Project));
			}
			if (ProjectItems.Num() == 0)
			{
				FText DialogText = FText::FromString("No projects found.");
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Projects loaded: %d"), ProjectItems.Num());
				ProjectListView->RefreshListItems();
			}
		},
		[](FString ErrorMessage) {
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage));
		}
	);
}
