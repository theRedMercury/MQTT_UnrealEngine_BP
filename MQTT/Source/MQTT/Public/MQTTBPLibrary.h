// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MQTTConnection.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MQTTBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/




UCLASS()
class AMQTTBPLibrary : public AActor
{
	GENERATED_BODY()

public:
	AMQTTBPLibrary();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	/*Initialize connection to MQTT server

!!! IF "Host" is empty : use 'DefaultGame.ini' [/Script/MQTT.MQTTConfig] :: IpServer !!!
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MQTT Init Connection", Keywords = "MQTT Initialisation"), Category = "MQTT")
	static bool MQTTInitConnection(int &index, FString ClientName = "MQTT_client", FString Host = "", FString Username = "", FString Password = "", int port = 1883, int keepalive = 5);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MQTT Close Connection", Keywords = "MQTT Close"), Category = "MQTT")
	static void MQTTCloseConnections();

	// Methods
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MQTT Publish", Keywords = "MQTT Publish"), Category = "MQTT")
	static bool MQTTPublish(int index = 0, FString topic = "", FString message = "", int qos = 0, bool retain = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MQTT Subscribe", Keywords = "MQTT Subscribe"), Category = "MQTT")
	static bool MQTTSubscribe(int index = 0, FString topic = "", int qos = 0);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MQTT Unsubscribe", Keywords = "MQTT Unsubscribe"), Category = "MQTT")
	static bool MQTTUnsubscribe(int index = 0, FString topic = "");


	// Events
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MQTT On Connect", Keywords = "MQTT OnUnsubscribe"), Category = "MQTT")
	void OnConnect(int index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MQTT On Disconnect", Keywords = "MQTT OnDisconnect"), Category = "MQTT")
	void OnDisconnect(int index);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MQTT On Publish", Keywords = "MQTT OnPublish"), Category = "MQTT")
	void OnPublish(int index, int mid);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MQTT On Message", Keywords = "MQTT OnMessage"), Category = "MQTT")
	void OnMessage(const int &index, const FString& topic, const FString& message, int qos, bool retain, int qosLenght);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MQTT On Subscribe", Keywords = "MQTT OnSubscribe"), Category = "MQTT")
	void OnSubscribe(int index, int mid, const TArray<int>& qos);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MQTT On Unsubscribe", Keywords = "MQTT OnUnsubscribe"), Category = "MQTT")
	void OnUnsubscribe(int index, int mid);

private:

	void MQTTLoopWorker();
};
