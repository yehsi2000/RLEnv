// Fill out your copyright notice in the Description page of Project Settings.

#include "RLAnimalCharacter.h"
#include "RLEnvGameModeBase.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


ARLAnimalCharacter::ARLAnimalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	isHungry = isThirsty = false;
}



TArray<FVector2D> ARLAnimalCharacter::See_Implementation()
{
	TArray<FVector2D> ret;
	return ret;
}

FVector2D ARLAnimalCharacter::Smell_Implementation()
{
	FVector2D ret;
	return ret;
}

void ARLAnimalCharacter::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
}

void ARLAnimalCharacter::Move() {
	if (action.Num() > 1) {
		if (MyController != nullptr) {
			FVector newloc = GetActorLocation() + FVector(action[0], action[1], 0) * MoveDist;
			MyController->MoveToLocation(newloc);
			DrawDebugPoint(GetWorld(), newloc, 7, FColor(255, 0, 0), false, 0.5f);
			UE_LOG(LogTemp, Log, TEXT("currentloc %f %f ,newloc %f %f"),GetActorLocation().X, GetActorLocation().Y, newloc.X, newloc.Y);
		}
		//AddActorWorldRotation(FRotator(0, action[1] * turnrate, 0));
		//AddMovementInput(GetActorForwardVector(), action[0] * movespeed);
		//SetActorLocation(GetActorLocation() + GetActorForwardVector().Normalize() * action[0] * movespeed);

	}
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Moved");
}

void ARLAnimalCharacter::BeginPlay()
{
	Super::BeginPlay();
	GameMode = (ARLEnvGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->Animals.Add(this);
	InitialTransform = GetTransform();
	MyController = Cast<AAIController>(GetController());
}

void ARLAnimalCharacter::ResetRL()
{
	Health = 100.f;
	Hunger = 100.f;
	isHungry = false;
	isThirsty = false;
	SetActorLocation(InitialTransform.GetLocation());
	SetActorRotation(InitialTransform.GetRotation());
}

void ARLAnimalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Hunger = FMath::Max(Hunger - HungerDecayRate, 0);
	//Thirst = FMath::Max(Thirst - ThirstDecayRate, 0);
	if (Hunger == 0) isHungry = true;
	//if (Thirst == 0) isThirsty = true;
	if (isHungry) Health -= HungerDamage;
	//if (isThirsty) Health -= ThirstDamage;
}
