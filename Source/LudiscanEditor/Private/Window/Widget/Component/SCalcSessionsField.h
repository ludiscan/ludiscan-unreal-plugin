#pragma once

class SCalcSessionsField: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCalcSessionsField) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args)
	{
		Content();
	}

	void SetSelectedProject(FProject Project)
	{
		SelectedProject = Project;
	}
private:
	
	FString CalcDataName = "";
	LudiscanClient Client = LudiscanClient();

	TArray<float> DataNames;

	FProject SelectedProject = FProject();

	void OnCalcDataNameChanged(const FText& Text)
	{
		CalcDataName = Text.ToString();
	}

	void OnCalcDataNameCommitted(const FText& Text, ETextCommit::Type Arg)
	{
		CalcDataName = Text.ToString();
	}

	void Content()
	{
		ChildSlot
		[
			SNew(SExpandableArea)
			.AreaTitle(FText::FromString("Calc Column"))
			.OnAreaExpansionChanged(this, &SCalcSessionsField::OnCalcAreaExpansionChanged)
			.BodyContent()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1)
					[
						SNew(SEditableTextBox)
						.Text_Lambda([this]() -> FText {
							return FText::FromString(CalcDataName);
						})
						.OnTextChanged(this, &SCalcSessionsField::OnCalcDataNameChanged)
						.OnTextCommitted(this, &SCalcSessionsField::OnCalcDataNameCommitted)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Calc Number Field"))
						.OnClicked(this, &SCalcSessionsField::OnCalcButtonClicked)
					]
				]
	
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					CalcResult()
				]
			]
		];
	}

	TSharedRef<SWidget> CalcResult()
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Calc Data Name"))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Average/"))
				]
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
					{
						if (DataNames.Num() == 0)
						{
							return FText::FromString("0");
						}
						float SumData = 0;
						for (float DataName : DataNames)
						{
							SumData += DataName;
						}
						return FText::FromString(FString::SanitizeFloat(SumData / DataNames.Num()));
					})
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Min/"))
				]
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
					{
						if (DataNames.Num() == 0)
						{
							return FText::FromString("0");
						}
						float MinData = DataNames.Num() > 0 ? DataNames[0] : 0;
						for (float DataName : DataNames)
						{
							MinData = FMath::Min(MinData, DataName);
						}
						return FText::FromString(FString::SanitizeFloat(MinData));
					})
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Max/"))
				]
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
					{
						if (DataNames.Num() == 0)
						{
							return FText::FromString("0");
						}
						float MaxData = DataNames.Num() > 0 ? DataNames[0] : 0;
						for (float DataName : DataNames)
						{
							MaxData = FMath::Max(MaxData, DataName);
						}
						return FText::FromString(FString::SanitizeFloat(MaxData));
					})
				]
			];
	}

	void OnCalcAreaExpansionChanged(bool bArg)
	{
		CalcDataName = "";
	}

	FReply OnCalcButtonClicked()
	{
		DataNames.Empty();
		if (SelectedProject.Id == 0)
		{
			FText DialogText = FText::FromString("Please select a project first.");
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			return FReply::Handled();
		}
		if (CalcDataName.IsEmpty())
		{
			FText DialogText = FText::FromString("Please input data name.");
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			return FReply::Handled();
		}
		Client.GetSessions(
			SelectedProject.Id,
			[this](TArray<FPlaySession> Sessions) {
				for (const FPlaySession& Session : Sessions)
				{
					for (const TPair<FString, FString> MetaDataPair: Session.MetaData)
					{
						if (MetaDataPair.Key.ToLower() == CalcDataName.ToLower())
						{
							if (MetaDataPair.Value.IsNumeric())
							{
								float Value = FCString::Atof(*MetaDataPair.Value);
								DataNames.Add(Value);
							}
						}
					}
					if (DataNames.Num() == 0)
					{
						FText DialogText = FText::FromString("No data found.");
						FMessageDialog::Open(EAppMsgType::Ok, DialogText);
						return;
					}
				
				}
			},
			[](FString ErrorMessage) {
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage));
			},
			200,
			0
		);
		return FReply::Handled();
	}
};
