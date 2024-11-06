// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RLAnimalCharacter.generated.h"

/**
 * 
 */


UCLASS()
class RLENV_API ARLAnimalCharacter : public ACharacter
{
	GENERATED_BODY()
	class ARLEnvGameModeBase* GameMode;

	class AAIController* MyController;

public: 
	ARLAnimalCharacter();

	void Tick(float DeltaTime) override;

	virtual void Move();

	FTransform InitialTransform;

	UFUNCTION(BlueprintNativeEvent)
	TArray<FVector2D> See();
	UFUNCTION(BlueprintNativeEvent)
	FVector2D Smell();
	UFUNCTION(BlueprintCallable)
	virtual void ToggleSprint();

	//TArray<float> CollectObservation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	TArray<float> action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	bool training = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MoveDist = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool isHungry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool isThirsty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float Hunger = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float Thirst = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float HungerDecayRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float ThirstDecayRate  = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float HungerDamage = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float ThirstDamage = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sensory")
	float fSightAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensory")
	float fSightRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sensory")
	float fOlfactoryRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mobility")
	float fWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mobility")
	float fSprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mobility")
	bool bIsSprinting;

	
public:
	void BeginPlay() override;

	void ResetRL();

};
