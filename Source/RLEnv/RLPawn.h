// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RLPawn.generated.h"

UCLASS()
class RLENV_API ARLPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARLPawn();

	UPROPERTY(EditAnywhere)
	APlayerController* PlayerController;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsConnectionOpen = false;
	bool WaitingForConnection = false;

	bool HasInput = false;
	bool HandledInput = false;
	bool AppliedInput = false;
	int Input_ = 0;
	TFuture<void> ClientConnectionFinishedFuture;
	TArray<uint8> Observation;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Open_Connection();
	void Close_Connection();
	void Conduct_Connection();
	virtual void CollectObservation();
	virtual void SendObservation();
	virtual void Reset_Pawn();
	virtual void HandleNetworkInput(int input);
	virtual void ApplyInput();
	virtual void CollectObservations();
	virtual void SendObservations();

	float Timer = 0;

	FSocket* ListenSocket;
	FSocket* ConnectionSocket;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
