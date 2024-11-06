// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RLEnvGameModeBase.generated.h"


typedef union NetByte{
	float f;
	uint8 byte[4];
};

UCLASS()
class RLENV_API ARLEnvGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TFuture<void> ClientConnectionFinishedFuture;

	bool IsConnectionOpen = false;
	bool WaitingForConnection = false;

	bool HasInput = false;
	bool HandledInput = false;
	bool AppliedInput = false;
	
	void BeginPlay() override;

public:

	ARLEnvGameModeBase();

	UPROPERTY(EditAnywhere, Category = "Training")
	bool isTraining;



	TArray<float> NNAction;
	int doReset = 0;
	float Observation[8]; //health, hunger, cur loc x,y, enemy sight x,y, food sight x,y
	TArray<class ARLAnimalCharacter*> Animals;

	void Open_Connection();
	void Close_Connection();
	void Conduct_Connection();

	void CollectObservation();
	void SendObservation();

	void Reset_Pawn();

	//void HandleNetworkInput(TArray<float> action);
	void ApplyInput();
	float Timer = 0;

	FSocket* ListenSocket;
	FSocket* ConnectionSocket;

	void Tick(float DeltaSeconds) override;

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

};
