#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Images/SImage.h"
#include "Client/FPlaySession.h"

class SSessionDetail: public SCompoundWidget
{
public:
		SLATE_BEGIN_ARGS(SSessionDetail) {}
    	SLATE_END_ARGS()
    
    	void Construct(const FArguments& Args)
		{
			ChildSlot
			.Padding(5.0f)
			[
				SNew(SWidgetSwitcher)
				.WidgetIndex(this, &SSessionDetail::GetWidgetIndex)
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
					SNew(SBorder)
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
					                    return FText::FromString(!Session.StartTime.IsEmpty()
					                        ? FString::Printf(TEXT("Start Time: %s"), *Session.StartTime)
					                        : FString(TEXT("Start Time: Unknown")));
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
					                    return FText::FromString(!Session.EndTime.IsEmpty()
					                        ? FString::Printf(TEXT("End Time: %s"), *Session.EndTime)
					                        : FString(TEXT("End Time: Unknown")));
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
				    ]
				]
			];
		}
	void SetSession(const FPlaySession& InSession)
	{
		Session = InSession;
	}
	void SetIsLoading(bool InIsLoading)
	{
		IsLoading = InIsLoading;
	}
private:

	FPlaySession Session = FPlaySession();
	bool IsLoading = false;
	
	int32 GetWidgetIndex() const
	{
		return IsLoading || (Session.SessionId == 0) ? 0 : 1;
	}
};
