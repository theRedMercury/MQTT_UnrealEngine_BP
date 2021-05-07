#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

#include <mosquittopp.h>

#include <string>
#include <queue>
#include <functional>
#include <sstream>

namespace MQTTUnreal {

	struct PublishEvent {
		std::string topic;
		std::string message;
		int qos;
		bool retain;
	};

	enum IncommingEventKey {
		OnConnect,
		OnDisconnect,
		OnPublish,
		OnMessage,
		OnSubscribe,
		OnUnsubscribe
	};

	struct IncommingEvent {
		IncommingEventKey type;
		std::string topic;
		std::string message;
		int messageLenght; // also 'mid'
		int qos;
		bool retain;
		TArray<int> aQos;
	};

	struct SubscribeEvent {
		std::string topic;
		int qos;
	};


	typedef std::function<void(IncommingEvent)> CallbackIE;

	class MQTTClient : public mosqpp::mosquittopp, public FRunnable
	{
	public:
		MQTTClient(const char* id);
		~MQTTClient();

		void setCallback(CallbackIE incommingCallback);

		// mosquittopp
		virtual void on_connect(int rc) override;
		//virtual void on_connect_with_flags(int rc, int flags) override;
		virtual void on_disconnect(int rc) override;
		virtual void on_publish(int mid) override;
		virtual void on_message(const struct mosquitto_message * message) override;
		virtual void on_subscribe(int mid, int qos_count, const int* granted_qos) override;
		virtual void on_unsubscribe(int mid) override;
		//virtual void on_log(int level, const char * str) override;
		//virtual void on_error() override;

		// FRunnable
		virtual bool Init() override;
		virtual uint32 Run() override;
		virtual void Stop() override;
		
		void publishNewEvent(PublishEvent ev);
		void subscribeNewEvent(SubscribeEvent se);
		void unSubscribeEvent(SubscribeEvent se);

	private:
		bool _running = false;

		std::queue<PublishEvent> * _publishEventQueue = NULL;
		std::queue<SubscribeEvent> * _subscribeEventQueue = NULL;

		FCriticalSection* _publishQueueLock = NULL;
		FCriticalSection* _subQueueLock = NULL;

		CallbackIE _incommingCallback;
	};
};