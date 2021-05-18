#pragma once

#include "MQTTConfigLegacy.generated.h"


UCLASS(config = EditorUserSettings, defaultconfig, meta = (DisplayName = "MQTT Settings"))
class UMQTTConfigLegacy : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, config, Category = "Variables")
		FString IpServer = "127.0.0.1";
};
