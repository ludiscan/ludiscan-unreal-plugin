#include "LudiscanPluginStyle.h"

#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir
TSharedPtr<FSlateStyleSet> FLudiscanPluginStyle::StyleInstance = nullptr;

void FLudiscanPluginStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLudiscanPluginStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLudiscanPluginStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("LudiscanPluginStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FLudiscanPluginStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("LudiscanPluginStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("LudiscanPlugin")->GetBaseDir() / TEXT("Resources"));

	Style->Set("LudiscanPlugin.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("my-icon"), Icon20x20));

	return Style;
}

void FLudiscanPluginStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}


const ISlateStyle& FLudiscanPluginStyle::Get()
{
	return *StyleInstance;
}