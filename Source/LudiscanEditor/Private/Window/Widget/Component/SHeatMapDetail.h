#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Images/SImage.h"
#include "Client/FPlaySession.h"

class SHeatMapDetail: public SCompoundWidget
{
public:
		SLATE_BEGIN_ARGS(SHeatMapDetail) {}
    	SLATE_END_ARGS()
    
    	void Construct(const FArguments& Args)
		{
			ChildSlot
			.Padding(5.0f)
			[
				SNew(SWidgetSwitcher)
				.WidgetIndex(this, &SHeatMapDetail::GetWidgetIndex)
				+ SWidgetSwitcher::Slot()
				[
					// ローディング中の表示
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SImage)
						.Image(FCoreStyle::Get().GetBrush("LoadingIndicator"))
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Loading..."))
					]
				]
				+ SWidgetSwitcher::Slot()
				[
					SessionDetail()
				]
				+ SWidgetSwitcher::Slot()
				[
					ProjectDetail()
				]
			];
		}
	void SetSession(const FPlaySession& InSession)
	{
		Session = InSession;
	}

	void SetProject(const FProject& InProject)
	{
		Project = InProject;
	}
	void SetIsLoading(bool InIsLoading)
	{
		IsLoading = InIsLoading;
	}
private:

	FPlaySession Session = FPlaySession();
	FProject Project = FProject();
	bool IsLoading = false;
	
	int32 GetWidgetIndex() const
	{
		if (IsLoading || (Session.SessionId == 0 && Project.Id == 0))
		{
			return 0;
		}
		if (Session.SessionId > 0)
		{
			return 1;
		}
		if (Project.Id > 0)
		{
			return 2;
		}
		return 0;
	}

	TSharedRef<SBorder> SessionDetail()
	{
		return SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f)) // 背景色
        .Padding(FMargin(5.0f))
        [
	        SNew(SVerticalBox)
	        + SVerticalBox::Slot()
	        [
	        	SNew(SExpandableArea)
				.AreaTitle(FText::FromString("Session Information"))
				.BodyContent()
		        [
			        SNew(SVerticalBox)
		            // セッション ID
		            + SVerticalBox::Slot()
		            .AutoHeight()
		            .Padding(2.0f)
		            [
		                SNew(STextBlock)
		                .Text_Lambda([this]() -> FText {
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
		                .Text_Lambda([this]() -> FText {
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
							.Text_Lambda([this]() -> FText {
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
		                .Text_Lambda([this]() -> FText {
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
		                .Text_Lambda([this]() -> FText {
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
		                .Text_Lambda([this]() -> FText {
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
		                .Text_Lambda([this]() -> FText {
		                    return FText::FromString(Session.StartTime.ToString());
		                })
		                .TextStyle(FAppStyle::Get(), "NormalText")
		            ]
		            // 終了時刻
		            + SVerticalBox::Slot()
		            .AutoHeight()
		            .Padding(2.0f)
		            [
		                SNew(STextBlock)
		                .Text_Lambda([this]() -> FText {
		                    return FText::FromString(Session.EndTime.ToString());
		                })
		                .TextStyle(FAppStyle::Get(), "NormalText")
		            ]
		        ]
	        ]
	        + SVerticalBox::Slot()
	        .AutoHeight()
	        .Padding(2.0f)
	        [
	        	SNew(SExpandableArea)
				.AreaTitle(FText::FromString("MetaData"))
		        .BodyContent()
		        [
		        	SNew(SVerticalBox)
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
							.Text_Lambda([this]() -> FText {
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
	        ]
	    ];
	}

	TSharedRef<SBorder> ProjectDetail() const
	{
		return SNew(SBorder)
		.BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f)) // 背景色
		.Padding(FMargin(5.0f))
		[
	        SNew(SVerticalBox)
	        + SVerticalBox::Slot()
	        [
	        	SNew(SExpandableArea)
				.AreaTitle(FText::FromString("Project Information"))
				.BodyContent()
		        [
			        SNew(SVerticalBox)
		            // プロジェクト ID
		            + SVerticalBox::Slot()
		            .AutoHeight()
		            .Padding(2.0f)
		            [
		                SNew(STextBlock)
		                .Text_Lambda([this]() -> FText {
		                    return FText::FromString(Project.Id > 0
		                        ? FString::Printf(TEXT("Project ID: %d"), Project.Id)
		                        : FString(TEXT("No project selected")));
		                })
		                .TextStyle(FAppStyle::Get(), "NormalText")
		            ]
		            // プロジェクト名
		            + SVerticalBox::Slot()
		            .AutoHeight()
		            .Padding(2.0f)
		            [
		                SNew(STextBlock)
		                .Text_Lambda([this]() -> FText {
		                    return FText::FromString(!Project.Name.IsEmpty()
		                        ? FString::Printf(TEXT("Name: %s"), *Project.Name)
		                        : FString(TEXT("Name: Unknown")));
		                })
		                .TextStyle(FAppStyle::Get(), "NormalText")
		            ]
		            // プロジェクト説明
		            + SVerticalBox::Slot()
		            .AutoHeight()
		            .Padding(2.0f)
		            [
		                SNew(STextBlock)
		                .Text_Lambda([this]() -> FText {
		                    return FText::FromString(!Project.Description.IsEmpty()
		                        ? FString::Printf(TEXT("Description: %s"), *Project.Description)
		                        : FString(TEXT("Description: Unknown")));
		                })
		                .TextStyle(FAppStyle::Get(), "NormalText")
		            ]
		            // 作成日時
		            + SVerticalBox::Slot()
		            .AutoHeight()
		            .Padding(2.0f)
		            [
		                SNew(STextBlock)
		                .Text_Lambda([this]() -> FText
		                {
			                return FText::FromString(Project.CreatedAt.ToString());
		                })
			        ]
		        ]
	        ]
		];
	}
};
