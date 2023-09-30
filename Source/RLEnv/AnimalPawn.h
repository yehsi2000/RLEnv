// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RLPawn.h"
#include "AnimalPawn.generated.h"

/**
 * 
 */
UCLASS()
class RLENV_API AAnimalPawn : public ARLPawn
{
	GENERATED_BODY()

	/**
	 * TODO : cone collider for sight
	 * sphere collider for smell
	 * TArray for saving sight info about enemy and food(vector value, vector with size of 0 when out of sight, 100 when right in front)
	 * value for saving smell strength(closer the stronger, scalar value)
	 * 
	 * or use ai perception
	 * 
	 * ����� �ܼ��� �̵����� ���ͷ� �ҰŸ� �������� �������̴°ŵ� ����ؾߵ�
	 * navmesh checking������?
	 * ����� behavior selector�� �ұ�
	 */
public:
	AAnimalPawn();

protected:
	UPROPERTY(EditAnywhere, Category="Sensory")
	float fSightAngle;

	UPROPERTY(EditAnywhere, Category="Sensory")
	float fSightRange;
	
	UPROPERTY(EditAnywhere, Category="Sensory")
	float fOlfactoryRange;

	UPROPERTY(EditAnywhere, Category="Mobility")
	float fWalkSpeed;

	UPROPERTY(EditAnywhere, Category="Mobility")
	float fSprintSpeed;

	UPROPERTY(EditAnywhere, Category="Mobility")
	bool bIsSprinting;

public:
	UFUNCTION()
	virtual void See();
	UFUNCTION()
	virtual void Smell();
	UFUNCTION()
	virtual void ToggleSprint();



	
};
