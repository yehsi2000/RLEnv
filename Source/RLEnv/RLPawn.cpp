#include "RLPawn.h"
#include "Async/Async.h"
#include "Sockets.h"
#include "Networking.h"

// Sets default values
ARLPawn::ARLPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARLPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());
	Open_Connection();
}

// Called every frame
void ARLPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!WaitingForConnection && PlayerController) {
		if (!HasInput) {
			PlayerController->SetPause(true);
		}
		if (PlayerController->IsPaused()) {
			UE_LOG(LogTemp, Log, TEXT("is paused"));
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("not paused, input = %d"), HasInput);
		}
	}
	ApplyInput();

	Conduct_Connection();

	if (AppliedInput) {
		CollectObservation();
		SendObservation();
	}
	
}

void ARLPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Close_Connection();
}

void ARLPawn::Open_Connection()
{
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

void ARLPawn::Close_Connection()
{
	if (IsConnectionOpen) {
		UE_LOG(LogTemp, Warning, TEXT("Closing Connection"));
		IsConnectionOpen = false;
		ListenSocket->Close();
	}
}

void ARLPawn::Conduct_Connection() {
	// Accept Connection
	if (WaitingForConnection) {
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		bool hasConnection = false;
		if (ListenSocket->HasPendingConnection(hasConnection) && hasConnection) {
			ConnectionSocket = ListenSocket->Accept(*RemoteAddress, TEXT("Connection"));
			WaitingForConnection = false;
			UE_LOG(LogTemp, Warning, TEXT("incoming connection"));

			// Start Recv Thread
			ClientConnectionFinishedFuture = Async(EAsyncExecution::ThreadPool, [&]() {
				UE_LOG(LogTemp, Warning, TEXT("recv thread started"));
				while (IsConnectionOpen) {
					uint32 size;
					TArray<uint8> ReceivedData;

					if (ConnectionSocket->HasPendingData(size)) {
						ReceivedData.Init(0, 10);
						int32 Read = 0;
						ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

						if (ReceivedData.Num() > 0)
						{
							Input_ = ReceivedData[0];
							HasInput = true;
							HandleNetworkInput(Input_);
						}
						
					}
				}
			});
		}
	}
}

void ARLPawn::CollectObservation()
{

}

void ARLPawn::SendObservation()
{
	int32 BytesSent = 0;
	ConnectionSocket->Send(Observation.GetData(), Observation.Num(), BytesSent);
	if (BytesSent != Observation.Num()) {
		UE_LOG(LogTemp, Warning, TEXT("Error during sending observation, obs size : %d, actual sent : %d"), Observation.Num(), BytesSent);
	}
}

void ARLPawn::Reset_Pawn()
{

}

void ARLPawn::HandleNetworkInput(int input)
{
	HandledInput = true;
}

void ARLPawn::ApplyInput()
{
	if (HandledInput) {
		HandledInput = false;
		AppliedInput = true;
	}
}

void ARLPawn::CollectObservations()
{

}

void ARLPawn::SendObservations()
{

}

// Called to bind functionality to input
void ARLPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

