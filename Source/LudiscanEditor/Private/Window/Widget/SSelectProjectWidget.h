#pragma once
#include "Client/LudiscanClient.h"
#include "Component/SFiltersList.h"

DECLARE_DELEGATE_OneParam(FOnProjectSelected, TSharedPtr<FProject>);

class LUDISCANEDITOR_API SSelectProjectWidget: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SSelectProjectWidget ) {}
	SLATE_EVENT(FOnProjectSelected, OnProjectSelected)
	SLATE_END_ARGS()
	void Construct(const FArguments& Args);

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
	TSharedPtr<SFiltersList<TSharedPtr<FProject>>> ProjectListView;
	LudiscanAPI::LudiscanClient Client = LudiscanAPI::LudiscanClient();
	FOnProjectSelected OnProjectSelected;

	void LoadProjects(const FString& HostName);

	void OnSelectionChanged(TSharedPtr<FProject> Item, ESelectInfo::Type) const
	{
		if (Item.IsValid() && OnProjectSelected.IsBound())
		{
			OnProjectSelected.Execute(Item);
		}
	}
};