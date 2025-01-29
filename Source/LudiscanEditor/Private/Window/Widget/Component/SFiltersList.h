#pragma once

#include "Fonts/FontMeasure.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "SFiltersList"

class SSearchBox;

constexpr float DefaultTextPadding = 42.f;

template <typename ItemType>
class SFiltersListRow : public SMultiColumnTableRow<ItemType>
{
public:
	using TColumnVisibility = TMap<FString, bool>;
	using SuperFArguments = typename SMultiColumnTableRow<ItemType>::FArguments;
	DECLARE_DELEGATE_RetVal_TwoParams(FString, FOnGetColumnString, ItemType, const FString&);

	SLATE_BEGIN_ARGS(SFiltersListRow)
        : _RowItem()
        , _ColumnVisibility()
        , _OnGetColumnString()
    {}
	SLATE_ARGUMENT(ItemType, RowItem)
	SLATE_ARGUMENT(TColumnVisibility*, ColumnVisibility)
	SLATE_EVENT(FOnGetColumnString, OnGetColumnString)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable)
	{
		RowItem           = InArgs._RowItem;
		ColumnVisibility  = InArgs._ColumnVisibility;
		OnGetColumnString = InArgs._OnGetColumnString;

		// 親クラスへ
		using Super = SMultiColumnTableRow<ItemType>;
		// FTableRowStyle RowStyle = FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
		// RowStyle.SetActiveBrush(FSlateColorBrush(FStyleColors::Header));
		Super::Construct(
			SuperFArguments(),
			// .Style(&RowStyle),
			OwnerTable);
	}

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnId) override
	{
		bool bShouldBeVisible = true;
		if (ColumnVisibility)
        {
            bShouldBeVisible = ColumnVisibility->FindRef(ColumnId.ToString());
        }

		if (!bShouldBeVisible)
		{
			return SNew(SBox).Visibility(EVisibility::Collapsed);
		}

		FString CellText;
		if (OnGetColumnString.IsBound())
		{
			CellText = OnGetColumnString.Execute(RowItem, ColumnId.ToString());
		}

		return SNew(STextBlock)
			.Text(FText::FromString(CellText))
			.Justification(ETextJustify::Type::Left);
	}

private:
	ItemType RowItem;
	TColumnVisibility* ColumnVisibility = nullptr; // ★ ポインタで保持
	FOnGetColumnString OnGetColumnString;
};

template <typename ItemType>
class SFiltersList: public SCompoundWidget
{
	using NullableItemType  = typename SListView<ItemType>::NullableItemType;
	
	using FOnSelectionChanged = typename SListView<ItemType>::FOnSelectionChanged;
	using FOnMouseButtonClick = typename SListView<ItemType>::FOnMouseButtonClick;
	using TColumnVisibility = typename SFiltersListRow<ItemType>::TColumnVisibility;
	using FOnGetColumnString = typename SFiltersListRow<ItemType>::FOnGetColumnString;
	DECLARE_DELEGATE_RetVal_OneParam(float, FOnGetColumnFixedWidth, const FString&);
	
public:
	SLATE_BEGIN_ARGS(SFiltersList)
		: _ItemHeight(16)
		, _ListItemsSource(nullptr)
		, _ColumnsSource(nullptr)
		, _SelectionMode(ESelectionMode::Single)
		, _FilterHintText()
		, _OnSelectionChanged()
		, _TableViewMode(ETableViewMode::List)
	{}
		SLATE_ARGUMENT(float, ItemHeight)
		SLATE_ARGUMENT(TArray<ItemType>*, ListItemsSource)
		SLATE_ARGUMENT(TArray<FString>*, ColumnsSource)
		SLATE_ARGUMENT(ESelectionMode::Type, SelectionMode)
		SLATE_ATTRIBUTE(FText, FilterHintText)
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
		SLATE_ARGUMENT(ETableViewMode::Type, TableViewMode)
		SLATE_EVENT(FOnGetColumnString, OnGetColumnString)
		SLATE_EVENT(FOnGetColumnFixedWidth, OnGetColumnFixedWidth)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		ItemHeight = InArgs._ItemHeight;
		SelectionMode = InArgs._SelectionMode;
		OnSelectionChanged = InArgs._OnSelectionChanged;
		TableViewMode = InArgs._TableViewMode;
		check(InArgs._OnGetColumnString.IsBound());
		OnGetColumnString = InArgs._OnGetColumnString;
		check(InArgs._OnGetColumnFixedWidth.IsBound());
		OnGetColumnFixedWidth = InArgs._OnGetColumnFixedWidth;
		ListItems = InArgs._ListItemsSource;
		FilterAndSortListItems();
		MakeColumnsSource(InArgs._ColumnsSource);

		FMenuBuilder FilterColumnOptions( true, nullptr);
		FilterColumnOptions.BeginSection("ViewOptions", FText::FromString("View Options"));
		for (FString& Column : *InArgs._ColumnsSource)
		{
			FilterColumnOptions.AddMenuEntry(
				FText::FromString(Column),
				FText::FromString(Column),
				FSlateIcon(),
				FUIAction( 
					FExecuteAction::CreateSPLambda(this, [this, Column]()
					{
						OnShowFilterColumnClicked(Column);
					}),
					FCanExecuteAction(),
					FIsActionChecked::CreateSPLambda(this, [this, Column]()
					{
						return IsShowFilterColumnChecked(Column);
					})
				),
				NAME_None,
				EUserInterfaceActionType::Check 
			);
		};

		TSharedRef<SSearchBox> SearchBox = SAssignNew(FilterTextBox, SSearchBox)
			.OnTextChanged(this, &SFiltersList::OnFilterTextChanged);
		SearchBox->SetHintText(InArgs._FilterHintText);
		SearchBox->SetHAlign(HAlign_Fill);

		HeaderRow = SNew(SHeaderRow);
		RegenerateColumns();

		SAssignNew(ListView, SListView<ItemType>)
			.ItemHeight(ItemHeight)
			.ListItemsSource(&FilteredSortedItems)
			.OnGenerateRow(this, &SFiltersList::OnGenerateRow)
			.SelectionMode(SelectionMode)
			.OnSelectionChanged(OnSelectionChanged)
			.AllowOverscroll(EAllowOverscroll::Yes)
			.ListViewStyle(FAppStyle::Get(), "ListView")
			.HeaderRow(HeaderRow.ToSharedRef());

		TSharedPtr<SWidget> FilterImage;
		{
			FilterImage = SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.Filter"))
				.ColorAndOpacity(FSlateColor::UseForeground())
				.ToolTipText(FText::FromString("Filter Options"));
		}

		TSharedRef<SHorizontalBox> FilterRowHBox = SNew(SHorizontalBox);
		FilterRowHBox->AddSlot()
			.FillWidth(1.f)
			.Padding(4)
			[
				SearchBox
			];

		FilterRowHBox->AddSlot()
			.Padding(0)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SComboButton)
				.HasDownArrow(false)
				.ContentPadding(0)
				.ForegroundColor(FSlateColor::UseForeground())
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewOptions")))
				.MenuContent()
				[
					FilterColumnOptions.MakeWidget()
				]
				.ButtonContent()
				[
					FilterImage.ToSharedRef()
				]
			];

		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8, 8)
			[
				FilterRowHBox
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				ListView.ToSharedRef()
			]
		];
	}

	void RefreshListItems()
	{
		FilterAndSortListItems();
		RequestListRefresh();
	}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
	SFiltersList(ETableViewMode::Type InListMode = ETableViewMode::List)
		: ItemHeight(16)
		, SelectionMode(ESelectionMode::Single)
		, TableViewMode(InListMode)
	{ 
#if WITH_ACCESSIBILITY
		AccessibleBehavior = EAccessibleBehavior::Auto;
		bCanChildrenBeAccessible = true;
#endif
	}
PRAGMA_ENABLE_DEPRECATION_WARNINGS
private:
	float ItemHeight;
	ESelectionMode::Type SelectionMode;
	FOnSelectionChanged OnSelectionChanged;
	ETableViewMode::Type TableViewMode;
	FOnGetColumnString OnGetColumnString;
	FOnGetColumnFixedWidth OnGetColumnFixedWidth;

	TArray<ItemType>* ListItems;
	TArray<ItemType> FilteredSortedItems;

	TSharedPtr<SSearchBox> FilterTextBox;
	TSharedPtr<SListView<ItemType>> ListView;
	TSharedPtr<SHeaderRow> HeaderRow;

	TSharedPtr<TColumnVisibility> FilterColumnVisibility;

	TPair<FString, EColumnSortMode::Type> CurrentSortColumn;

	FString FilterRowText;

	void RequestListRefresh()
	{
		RegenerateColumns();
		HeaderRow->RefreshColumns();
		ListView->RequestListRefresh();
	}

	void MakeColumnsSource(const TArray<FString>* Columns)
	{
		check(Columns->Num() > 0);
		FilterColumnVisibility = MakeShareable(new TMap<FString, bool>());
		FilterColumnVisibility->Empty();
		for (FString Column : *Columns)
		{
			FString ColumnName = Column;
			FilterColumnVisibility->Add(ColumnName, true);
		} 
	}

	void OnShowFilterColumnClicked(const FString& Column)
	{
		FilterColumnVisibility->FindOrAdd(Column) = !FilterColumnVisibility->FindChecked(Column);
		FilterAndSortListItems();
		RequestListRefresh();
	}

	bool IsShowFilterColumnChecked(const FString& Column) const
	{
		return FilterColumnVisibility->FindChecked(Column);
	}

	void OnSortByColumn(const EColumnSortPriority::Type, const FName& ColumnId, const EColumnSortMode::Type)
	{
		CurrentSortColumn = TPair<FString, EColumnSortMode::Type>(ColumnId.ToString(), EColumnSortMode::Ascending);
		FilterAndSortListItems();
		RequestListRefresh();
	}

	EVisibility GetColumnVisibility(const FString& Column) const
    {
        return FilterColumnVisibility->FindChecked(Column) ? EVisibility::Visible : EVisibility::Collapsed;
    }

	void FilterAndSortListItems()
	{
		FilteredSortedItems.Reset();
		FilteredSortedItems = ListItems->FilterByPredicate([&](const ItemType& Item) {
			if (FilterRowText.IsEmpty())
			{
				return true;
			}
			for (TPair<FString, bool> Column : *FilterColumnVisibility)
			{
				if (Column.Value && OnGetColumnString.IsBound())
				{
					FString ColumnString = OnGetColumnString.Execute(Item, Column.Key);
					if (ColumnString.Contains(FilterRowText))
					{
						return true;
					}
				}
			}
			return false;
		});
		if (!CurrentSortColumn.Key.IsEmpty() && OnGetColumnString.IsBound())
		{
			FilteredSortedItems.Sort([this](const ItemType& A, const ItemType& B) {
				FString AString = OnGetColumnString.Execute(A, CurrentSortColumn.Key);
				FString BString = OnGetColumnString.Execute(B, CurrentSortColumn.Key);
				return CurrentSortColumn.Value == EColumnSortMode::Ascending ? (AString < BString) : (AString > BString);
			});	
		}
	}

	EColumnSortMode::Type GetSortMode(const FString& ColumnId) const
	{
		return (CurrentSortColumn.Key == ColumnId) ? CurrentSortColumn.Value : EColumnSortMode::None;
	}

	void RegenerateColumns()
	{
		HeaderRow->ClearColumns();

	    // まずはフォントメジャーを取得
	    TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	    FSlateFontInfo FontInfo = FAppStyle::Get().GetFontStyle("NormalText");

	    // 「各カラムの最大幅」を計算
	    // 注意: FilteredSortedItems が空の場合は 100.0f などのデフォルト幅にしておく
	    TMap<FString, float> ColumnMaxWidths;
	    for (const TPair<FString, bool>& ColumnVisibilityPair : *FilterColumnVisibility)
	    {
	        if (!ColumnVisibilityPair.Value) 
	        {
	            // 非表示カラムはスキップ
	            continue;
	        }
	        const FString& ColumnName = ColumnVisibilityPair.Key;

	        float MaxWidth = 100.f; // デフォルト値
	        if (FilteredSortedItems.Num() > 0)
	        {
	            MaxWidth = 0.f;
	            for (const ItemType& Item : FilteredSortedItems)
	            {
	                FString Text = OnGetColumnString.Execute(Item, ColumnName);
	                FVector2D TextSize = FontMeasure->Measure(Text, FontInfo);
	                if (TextSize.X > MaxWidth)
	                {
	                    MaxWidth = TextSize.X;
	                }
	            }
	            // 多少の余白をプラス
	            MaxWidth += DefaultTextPadding;
	        }

	        ColumnMaxWidths.Add(ColumnName, MaxWidth);
	    }

	    // 次に AddColumn
	    for (const TPair<FString, bool>& ColumnVisibilityPair : *FilterColumnVisibility)
	    {
	        if (ColumnVisibilityPair.Value)
	        {
	            const FString& Column = ColumnVisibilityPair.Key;

	            HeaderRow->AddColumn(
	                SHeaderRow::Column(FName(*Column))
	                .DefaultLabel(FText::FromString(Column))
	                .FixedWidth(ColumnMaxWidths[Column]) // ← ここで設定
	                .OnSort(this, &SFiltersList::OnSortByColumn)
	                .SortMode_Lambda([this, Column]() -> EColumnSortMode::Type
	                {
	                    return GetSortMode(Column);
	                })
	                .HAlignCell(HAlign_Center)
	                .VAlignCell(VAlign_Center)
	            );
	        }
	    }
	}

	TSharedRef<ITableRow> OnGenerateRow(ItemType InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		return SNew(SFiltersListRow<ItemType>, OwnerTable)
			.RowItem(InItem)
			.ColumnVisibility(FilterColumnVisibility.Get())
			.OnGetColumnString(OnGetColumnString);
	}

	void OnFilterTextChanged(const FText& NewText)
	{
		FString FilterText = NewText.ToString();
		FilterRowText = FilterText;
		FilterAndSortListItems();
		RequestListRefresh();
	}
};

#undef LOCTEXT_NAMESPACE
