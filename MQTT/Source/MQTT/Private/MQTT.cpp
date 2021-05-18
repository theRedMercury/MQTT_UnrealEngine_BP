// Copyright Epic Games, Inc. All Rights Reserved.

#include "MQTT.h"

#if ENGINE_MINOR_VERSION < 26
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "MQTTConfigLegacy.h"
#endif

#define LOCTEXT_NAMESPACE "FMQTTModule"

void FMQTTModule::StartupModule()
{
#if ENGINE_MINOR_VERSION < 26
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		TSharedPtr<ISettingsContainer> ProjectSettingsContainer = SettingsModule->GetContainer("Project");
		ProjectSettingsContainer->DescribeCategory("MQTT", FText::FromString("MQTT"), FText::FromString("MQTT settings"));

		SettingsModule->RegisterSettings("Project", "MQTT", "MQTT settings",
			FText::FromString("MQTT Settings"),
			FText::FromString("Configure MQTT Settings"),
			GetMutableDefault<UMQTTConfigLegacy>()
		);

	}
#endif
	
}

void FMQTTModule::ShutdownModule()
{
#if ENGINE_MINOR_VERSION < 26
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Project", "MQTT", "MQTT");
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMQTTModule, MQTT)