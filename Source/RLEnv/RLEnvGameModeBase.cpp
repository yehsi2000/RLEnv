// Copyright Epic Games, Inc. All Rights Reserved.

#include "RLEnvGameModeBase.h"
#include "Async/Async.h"
#include "Sockets.h"
#include "Networking.h"
#include "Kismet/GameplayStatics.h"
#include "RLAnimalCharacter.h"

ARLEnvGameModeBase::ARLEnvGameModeBase() {
	PrimaryActorTick.bCanEverTick = true;
	SetTickableWhenPaused(true);
}

void ARLEnvGameModeBase::Open_Connection() {
	if (!IsConnectionOpen) {
		UE_LOG(LogTemp, Warning, TEXT("Openning Connection"));
		IsConnectionOpen = true;
		WaitingForConnection = true;

		FIPv4Address IPAddress;
		FIPv4Address::Parse(FString("127.0.0.1"), IPAddress);
		FIPv4Endpoint Endpoint(IPAddress, (uint16)7794);

		ListenSocket = FTcpSocketBuilder(TEXT("TcpSocket")).AsReusable();

		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		TSharedRef<FInternetAddr> addr = SocketSubsystem->CreateInternetAddr();
		addr->SetIp(Endpoint.Address.Value);
		addr->SetPort(Endpoint.Port);
		ListenSocket->Bind(*addr);
		//ListenSocket->Bind(*SocketSubsystem->CreateInternetAddr(Endpoint.Address.Value, Endpoint.Port));
		ListenSocket->Listen(1);
		UE_LOG(LogTemp, Warning, TEXT("Listening"));
	}
}

void ARLEnvGameModeBase::Close_Connection() {
	if (IsConnectionOpen) {
		UE_LOG(LogTemp, Warning, TEXT("Closing Connection"));
		IsConnectionOpen = false;
		ListenSocket->Close();
	}
}

void ARLEnvGameModeBase::Conduct_Connection() {
	// Accept Connection
	if (WaitingForConnection) {
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool hasConnection = false;
		if (ListenSocket->HasPendingConnection(hasConnection) && hasConnection) {
			ConnectionSocket = ListenSocket->Accept(*RemoteAddress, TEXT("Connection"));
			WaitingForConnection = false;
			UE_LOG(LogTemp, Log, TEXT("Connected!"));
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, TEXT("Connected!"));

			// Start Recv Thread
			ClientConnectionFinishedFuture = Async(EAsyncExecution::ThreadPool, [&]() {
				UE_LOG(LogTemp, Warning, TEXT("recv thread started"));
				while (IsConnectionOpen) {
					uint32 size;
					uint8 ReceivedData[12];

					if (ConnectionSocket->HasPendingData(size)) {
						//ReceivedData.Init(0, 10);
						int32 Read = 0;
						ConnectionSocket->Recv(ReceivedData, 12, Read);
						if (Read == 12) {
							float receivedaction[3];
							for (int i = 0; i < 3; i++) {
								union { uint8 b[4]; float f; } u;
								u.b[0] = ReceivedData[i * 4];
								u.b[1] = ReceivedData[i * 4 + 1];
								u.b[2] = ReceivedData[i * 4 + 2];
								u.b[3] = ReceivedData[i * 4 + 3];
								receivedaction[i] = u.f;
							}
							UE_LOG(LogTemp, Log, TEXT("received %f %f %f "),
								receivedaction[0], receivedaction[1], receivedaction[2]);
							NNAction.Empty();
							NNAction.Append(receivedaction, 2);
							HasInput = true;
							doReset = receivedaction[2];
							if (doReset == 0 && Animals.Num() > 0) {
								UE_LOG(LogTemp, Log, TEXT("setting action to animal"));
								for (ARLAnimalCharacter* Animal : Animals) {
									if (Animal->training) Animal->action = NNAction;
								}
								HandledInput = true;
							}

						} else {
							UE_LOG(LogTemp, Warning, TEXT("Error during receiving, received : %d"), Read);
						}

					}
				}
			});
		}
	}
}

void ARLEnvGameModeBase::CollectObservation() {
	UE_LOG(LogTemp, Log, TEXT("collecting obs"));
	for (ARLAnimalCharacter* Animal : Animals) {
		if (!Animal->training) continue;

		Observation[0] = Animal->Health / 10;
		Observation[1] = Animal->Hunger / 10;
		Observation[2] = Animal->GetActorLocation().X / 300.f;
		Observation[3] = Animal->GetActorLocation().Y / 300.f;
		TArray<FVector2D> visualvec = Animal->See();
		if (visualvec.Num() >= 1) {
			Observation[4] = Animal->See()[0].X; //predator
			Observation[5] = Animal->See()[0].Y;
			Observation[6] = Animal->See()[1].X; //food
			Observation[7] = Animal->See()[1].Y;
		} else {
			Observation[4] = 0;
			Observation[5] = 0;
			Observation[6] = 0;
			Observation[7] = 0;
		}
	}
}

void ARLEnvGameModeBase::SendObservation() {
	int32 BytesSent = 0;
	uint8 ByteObservation[32] = { 0, };
	//ByteObservation.AddUninitialized(32);
	UE_LOG(LogTemp, Log, TEXT("sending %f %f %f %f %f %f %f %f"),
		Observation[0], Observation[1], Observation[2], Observation[3],
		Observation[4], Observation[5], Observation[6], Observation[7]);
	FMemory::Memcpy(ByteObservation, Observation, sizeof(float)*8);

	ConnectionSocket->Send(ByteObservation, 32, BytesSent);
	if (BytesSent != 32) {
		UE_LOG(LogTemp, Warning, TEXT("Error during sending observation, obs size : %d, actual sent : %d"), 32, BytesSent);
	}
}

void ARLEnvGameModeBase::Reset_Pawn() {
	UE_LOG(LogTemp, Log, TEXT("Resetting..."));
	for (ARLAnimalCharacter* Animal : Animals) {
		Animal->ResetRL();
	}
}

void ARLEnvGameModeBase::ApplyInput() {
	if (doReset > 0) {
		doReset = 0;
		Reset_Pawn();
		AppliedInput = true;
	}
	if (HandledInput) {
		UE_LOG(LogTemp, Log, TEXT("Applying input"));
		HandledInput = false;
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		for (ARLAnimalCharacter* Animal : Animals) {
			if (Animal->training) Animal->Move();
		}
		AppliedInput = true;
	}
}

void ARLEnvGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (!isTraining) return;
	if (!WaitingForConnection) {
		if (!HasInput) {
			//UE_LOG(LogTemp, Log, TEXT("Waiting for input"));
			GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, TEXT("Waiting for input"));
			//UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, FString::Printf(TEXT("Paused: %s"), UGameplayStatics::IsGamePaused(GetWorld()) ? TEXT("true") : TEXT("false")));
		//if (UGameplayStatics::IsGamePaused(GetWorld())) {UE_LOG(LogTemp, Log, TEXT("is paused"));}
		//else {UE_LOG(LogTemp, Log, TEXT("not paused, input = %d"), HasInput);}

	}
	ApplyInput();

	Conduct_Connection();

	if (AppliedInput) {
		CollectObservation();
		SendObservation();
	}

	if (WaitingForConnection) {
		//GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, TEXT("Waiting for connection..."));
		//Cast<APlayerController>(Animals[0]->GetController()).SetPause(true);
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

	if (HasInput) HasInput = false;
}

void ARLEnvGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	Super::InitGame(MapName, Options, ErrorMessage);
	//Open_Connection();
}

void ARLEnvGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	Close_Connection();
}

void ARLEnvGameModeBase::BeginPlay() {
	Super::BeginPlay();
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	Open_Connection();
	doReset = 0;
}
