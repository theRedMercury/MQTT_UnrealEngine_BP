#pragma once

#include "Engine/DeveloperSettings.h"
#include "MQTTConfig.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "MQTT Settings"))
class UMQTTConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, config, Category = "Variables")
		FString IpServer = "127.0.0.1";
};
