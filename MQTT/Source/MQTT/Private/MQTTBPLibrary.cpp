// Copyright Epic Games, Inc. All Rights Reserved.

#include "MQTTBPLibrary.h"

AMQTTBPLibrary::AMQTTBPLibrary()
{
	// Disable event tick
	PrimaryActorTick.bCanEverTick = false;
}

void AMQTTBPLibrary::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle timer_handle;
	GetWorldTimerManager().SetTimer(timer_handle, this, &AMQTTBPLibrary::MQTTLoopWorker, 0.1f, true);
}

void AMQTTBPLibrary::BeginDestroy()
{
	MQTTCloseConnections();
	Super::BeginDestroy();
}

bool AMQTTBPLibrary::MQTTInitConnection(int &index, FString ClientName, FString  Host, FString  Username, FString  Password, int port, int keepalive)
{
	MQTTUnreal::MQTTConnection * newConnection = new MQTTUnreal::MQTTConnection(
		std::string(TCHAR_TO_ANSI(*ClientName)),
		std::string(TCHAR_TO_ANSI(*Host)),
		std::string(TCHAR_TO_ANSI(*Username)),
		std::string(TCHAR_TO_ANSI(*Password)),
		port,
		keepalive);

	if (newConnection->incommingEventQueue != NULL) {
		MQTTUnreal::connections.push_back(newConnection);
		index = MQTTUnreal::connections.size() - 1;
		return true;
	}
	index = -1;
	return false;
}

void AMQTTBPLibrary::MQTTCloseConnections()
{
	for (auto iter = MQTTUnreal::connections.begin(); iter != MQTTUnreal::connections.end(); ) {
		delete(*iter);
		iter = MQTTUnreal::connections.erase(iter);
	}
	MQTTUnreal::connections.clear();
}

bool AMQTTBPLibrary::MQTTPublish(int index, FString topic, FString message, int qos, bool retain)
{
	if (!MQTTUnreal::connections.empty() && index < MQTTUnreal::connections.size()) {
		return MQTTUnreal::connections.at(index)->publish(
			std::string(TCHAR_TO_ANSI(*topic)),
			std::string(TCHAR_TO_ANSI(*message)),
			qos,
			retain
			);
	}
	return false;
}

bool AMQTTBPLibrary::MQTTSubscribe(int index, FString topic, int qos)
{
	if (!MQTTUnreal::connections.empty() && index < MQTTUnreal::connections.size()) {
		return MQTTUnreal::connections.at(index)->subscribe(
			std::string(TCHAR_TO_ANSI(*topic)),
			qos
			);
	}
	return false;
}

bool AMQTTBPLibrary::MQTTUnsubscribe(int index, FString topic)
{
	if (!MQTTUnreal::connections.empty() && index < MQTTUnreal::connections.size()) {
		return MQTTUnreal::connections.at(index)->unSubscribe(
			std::string(TCHAR_TO_ANSI(*topic))
			);
	}
	return false;
}

/* Event Process */
void AMQTTBPLibrary::MQTTLoopWorker()
{
	int index = 0;
	for (auto & connection : MQTTUnreal::connections) {

		if (connection->incommingEventLock != NULL) {
			connection->incommingEventLock->Lock();

			while (!connection->incommingEventQueue->empty()) {

				MQTTUnreal::IncommingEvent ie = connection->incommingEventQueue->front();
				connection->incommingEventQueue->pop();

				switch (ie.type)
				{
				case MQTTUnreal::IncommingEventKey::OnConnect:
					OnConnect(index);
					break;

				case MQTTUnreal::IncommingEventKey::OnDisconnect:
					OnDisconnect(index);
					break;

				case MQTTUnreal::IncommingEventKey::OnPublish:
					OnPublish(index, ie.messageLenght);
					break;
					
				case MQTTUnreal::IncommingEventKey::OnMessage:
					OnMessage(index, FString(ie.topic.c_str()), FString(ie.message.c_str()), ie.qos, ie.retain, ie.messageLenght);
					break;

				case MQTTUnreal::IncommingEventKey::OnSubscribe:
					OnSubscribe(index, ie.messageLenght, ie.aQos);
					break;

				case MQTTUnreal::IncommingEventKey::OnUnsubscribe:
					OnUnsubscribe(index, ie.messageLenght);
					break;

				default:
					break;
				}
			}
			connection->incommingEventLock->Unlock();
		}
		index += 1;
	}
}
