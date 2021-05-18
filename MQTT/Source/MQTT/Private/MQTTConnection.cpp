#include "MQTTConnection.h"

MQTTUnreal::MQTTConnection::MQTTConnection(std::string cliName, std::string host, std::string username, std::string pwd, int port, int keepalive)
{
	
#if ENGINE_MINOR_VERSION >= 26
	//UE_LOG(LogTemp, Warning, TEXT("UMQTTConfig::ip = %s"), *GetDefault<UMQTTConfig>()->IpServer);
	std::string _host = TCHAR_TO_ANSI(*FString(*GetDefault<UMQTTConfig>()->IpServer));
#else
	std::string _host = TCHAR_TO_ANSI(*FString(*GetDefault<UMQTTConfigLegacy>()->IpServer));
#endif

	if (!host.empty()) {
		_host = host;
	}

	//UE_LOG(LogTemp, Warning, TEXT("MQTT connect::ip = %s"), *FString(_host.c_str()));
	_mqttClient = new MQTTClient(cliName.c_str());
	_mqttClient->max_inflight_messages_set(0);

	// User & password
	if (!username.empty()) {
		_mqttClient->username_pw_set(username.c_str(), pwd.c_str());
	}

	// MQTT Connection
	int rc = _mqttClient->connect(_host.c_str(), port, keepalive);
	if (rc != 0) {
		UE_LOG(LogTemp, Error, TEXT("MQTT connect error: %s"), ANSI_TO_TCHAR(mosquitto_strerror(rc)));
		delete _mqttClient;
		_mqttClient = NULL;
	}
	else {
		incommingEventQueue = new std::queue<MQTTUnreal::IncommingEvent>();
		incommingEventLock = new FCriticalSection();

		_mqttClient->setCallback([this](IncommingEvent i) { incommingCallback(i); });
		_runThread = FRunnableThread::Create(_mqttClient, TEXT("MQTTUnreal"), 0, TPri_Normal, FGenericPlatformAffinity::GetNoAffinityMask());
	}
};

MQTTUnreal::MQTTConnection::~MQTTConnection()
{
	if (_mqttClient != NULL) {
		_runThread->Kill();
		_runThread->WaitForCompletion();

		delete _mqttClient;
		delete _runThread;
		delete incommingEventQueue;
		delete incommingEventLock;

		_mqttClient = NULL;
		_runThread = NULL;
		incommingEventQueue = NULL;
		incommingEventLock = NULL;
	}
};

void MQTTUnreal::MQTTConnection::incommingCallback(IncommingEvent ie)
{
	incommingEventLock->Lock();
	incommingEventQueue->push(ie);
	incommingEventLock->Unlock();
}

bool MQTTUnreal::MQTTConnection::publish(std::string topic, std::string message, int qos, bool retain)
{
	if (_mqttClient != NULL) {
		_mqttClient->publishNewEvent(MQTTUnreal::PublishEvent{ topic, message, qos, retain });
		return true;
	}
	return false;
}

bool  MQTTUnreal::MQTTConnection::subscribe(std::string topic, int qos)
{
	if (_mqttClient != NULL) {
		_mqttClient->subscribeNewEvent(MQTTUnreal::SubscribeEvent{ topic, qos });
		return true;
	}
	return false;
}

bool  MQTTUnreal::MQTTConnection::unSubscribe(std::string topic)
{
	if (_mqttClient != NULL) {
		_mqttClient->unSubscribeEvent(MQTTUnreal::SubscribeEvent { topic, -1 });
		return true;
	}
	return false;
}