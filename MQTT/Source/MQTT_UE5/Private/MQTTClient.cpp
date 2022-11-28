#include "MQTTClient.h"

DEFINE_LOG_CATEGORY(MQTTPluginLog);

MQTTUnreal::MQTTClient::MQTTClient(const char * id) : mosqpp::mosquittopp(id)
{
	mosqpp::lib_init();
	_publishEventQueue = new std::queue<MQTTUnreal::PublishEvent>();
	_subscribeEventQueue = new std::queue<MQTTUnreal::SubscribeEvent>();
	_publishQueueLock = new FCriticalSection();
	_subQueueLock = new FCriticalSection();
}

MQTTUnreal::MQTTClient::~MQTTClient()
{
	this->Stop();
	delete _publishEventQueue;
	delete _subscribeEventQueue;
	delete _publishQueueLock;
	delete _subQueueLock;

	mosqpp::mosquittopp::disconnect();
	mosqpp::lib_cleanup();

	_publishEventQueue = NULL;
	_subscribeEventQueue = NULL;
	_publishQueueLock = NULL;
	_subQueueLock = NULL;
}

void MQTTUnreal::MQTTClient::setCallback(CallbackIE incommingCallback){
	_incommingCallback = incommingCallback;
}

void MQTTUnreal::MQTTClient::publishNewEvent(PublishEvent ev)
{
	_publishQueueLock->Lock();
	_publishEventQueue->push(ev);
	_publishQueueLock->Unlock();
}

void MQTTUnreal::MQTTClient::subscribeNewEvent(SubscribeEvent se)
{
	_subQueueLock->Lock();
	_subscribeEventQueue->push(se);
	_subQueueLock->Unlock();
}

void MQTTUnreal::MQTTClient::unSubscribeEvent(SubscribeEvent se)
{
	_subQueueLock->Lock();
	_subscribeEventQueue->push(se);
	_subQueueLock->Unlock();
}

void MQTTUnreal::MQTTClient::on_connect(int rc)
{
	if (rc != 0) {
		UE_LOG(MQTTPluginLog, Error, TEXT("on_connect error: %s"), ANSI_TO_TCHAR(mosqpp::strerror(rc)));
		return;
	}
	UE_LOG(MQTTPluginLog, Log, TEXT("MQTTClient:: on_connect"));

	_incommingCallback(IncommingEvent { MQTTUnreal::IncommingEventKey::OnConnect, "", "", rc });
}

void MQTTUnreal::MQTTClient::on_disconnect(int rc)
{
	if (rc != 0) {
		UE_LOG(MQTTPluginLog, Error, TEXT("on_disconnect error: %s"), ANSI_TO_TCHAR(mosqpp::strerror(rc)));
		return;
	}
	UE_LOG(MQTTPluginLog, Log, TEXT("MQTTClient:: on_disconnect"));

	_incommingCallback(IncommingEvent { MQTTUnreal::IncommingEventKey::OnDisconnect, "", "", rc });
}

void MQTTUnreal::MQTTClient::on_publish(int mid)
{
	_incommingCallback(IncommingEvent { MQTTUnreal::IncommingEventKey::OnPublish, "", "", mid });
}

void MQTTUnreal::MQTTClient::on_message(const mosquitto_message * src)
{
	std::stringstream ss;
	char* payload = (char*)src->payload;
	for (int i = 0; i < src->payloadlen; i++) ss << (char)(payload[i]);

	_incommingCallback(IncommingEvent{ MQTTUnreal::IncommingEventKey::OnMessage, std::string(src->topic), ss.str(), src->qos, src->retain });
}

void MQTTUnreal::MQTTClient::on_subscribe(int mid, int qos_count, const int * granted_qos)
{
	TArray<int> qos;
	for (auto p = granted_qos; p < granted_qos + qos_count; ++p) {
		qos.Add(*p);
	}
	_incommingCallback(IncommingEvent{ MQTTUnreal::IncommingEventKey::OnSubscribe, "", "", mid, 0, false, qos });
}

void MQTTUnreal::MQTTClient::on_unsubscribe(int mid)
{
	_incommingCallback(IncommingEvent{ MQTTUnreal::IncommingEventKey::OnSubscribe, "", "", mid });
}

bool MQTTUnreal::MQTTClient::Init()
{
	_running = true;
	return _running;
}

uint32 MQTTUnreal::MQTTClient::Run()
{
	int rc;	
	while (_running)
	{
		// Publish message
		while (_publishEventQueue != NULL && !_publishEventQueue->empty()) {
			_publishQueueLock->Lock();
			PublishEvent ev = _publishEventQueue->front();
			_publishEventQueue->pop();
			_publishQueueLock->Unlock();

			void* payload = malloc(ev.message.length());
			memcpy(payload, ev.message.c_str(), ev.message.length());

			mosqpp::mosquittopp::publish(NULL, ev.topic.c_str(), ev.message.length(), payload, ev.qos, ev.retain);
			free(payload);
		}

		// Sub event
		while (_subscribeEventQueue != NULL && !_subscribeEventQueue->empty()) {
			_subQueueLock->Lock();
			SubscribeEvent se = _subscribeEventQueue->front();
			_subscribeEventQueue->pop();
			_subQueueLock->Unlock();

			if (se.qos == -1) {
				mosqpp::mosquittopp::unsubscribe(NULL, se.topic.c_str());
			}
			else {
				mosqpp::mosquittopp::subscribe(NULL, se.topic.c_str(), se.qos);
			}
		}

		// Loop
		rc = mosqpp::mosquittopp::loop();
		if (rc != 0) {
			UE_LOG(MQTTPluginLog, Error, TEXT("Mosquitto loop error: %s"), ANSI_TO_TCHAR(mosqpp::strerror(rc)));
			mosqpp::mosquittopp::reconnect();
		}
	}
	return 0;
}

void MQTTUnreal::MQTTClient::Stop()
{
	_running = false;
}
