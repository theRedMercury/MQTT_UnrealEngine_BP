#pragma once

#include "CoreMinimal.h"

#include "MQTTConfig.h"
#include "MQTTClient.h"

#include <vector>
#include <functional>

namespace MQTTUnreal {

	class MQTTConnection {

	public:
		MQTTConnection(std::string cliName, std::string host, std::string username, std::string pwd, int port, int keepalive);
		~MQTTConnection();

		bool publish(std::string topic, std::string message, int qos = 0, bool retain = false);
		bool subscribe(std::string topic, int qos = 0);
		bool unSubscribe(std::string topic);

		void incommingCallback(IncommingEvent i);

		FCriticalSection* incommingEventLock = NULL;
		std::queue<IncommingEvent> * incommingEventQueue = NULL;

	private:
		MQTTClient * _mqttClient = NULL;
		FRunnableThread * _runThread = NULL;
	};

	static std::vector<MQTTUnreal::MQTTConnection *> connections; // Main connections vector BP
}
