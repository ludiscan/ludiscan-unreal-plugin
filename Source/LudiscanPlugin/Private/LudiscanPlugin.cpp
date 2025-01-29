// Copyright Epic Games, Inc. All Rights Reserved.

#include "LudiscanPlugin.h"

#define LOCTEXT_NAMESPACE "FLudiscanPluginModule"

DEFINE_LOG_CATEGORY(LogLudiscanAPI);


void FLudiscanPluginModule::StartupModule()
{
	
}

void FLudiscanPluginModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLudiscanPluginModule, LudiscanPlugin)