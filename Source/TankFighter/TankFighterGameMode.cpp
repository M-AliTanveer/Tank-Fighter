// Copyright Epic Games, Inc. All Rights Reserved.

#include "TankFighterGameMode.h"
#include "TankFighterPlayerController.h"
#include "TankFighterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TankFighterGameState.h"
#include "Kismet/GameplayStatics.h"


ATankFighterGameMode::ATankFighterGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATankFighterPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TankFighter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		tankClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TankFighterController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	GameStateClass = ATankFighterGameState::StaticClass();
}

void ATankFighterGameMode::BeginPlay()
{

	Super::BeginPlay();

	gameStateRef = Cast<ATankFighterGameState>(UGameplayStatics::GetGameState(GetWorld()));

}

void ATankFighterGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ATankFighterPlayerController* newPlayerCasted = Cast<ATankFighterPlayerController>(NewPlayer);

	SpawnAndPosses(newPlayerCasted);
	if (gameStateRef)
		gameStateRef->PlayerJoined(newPlayerCasted);
	else
	{
		gameStateRef = Cast<ATankFighterGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if(gameStateRef)
			gameStateRef->PlayerJoined(newPlayerCasted);

	}

}

void ATankFighterGameMode::SpawnAndPosses_Implementation(ATankFighterPlayerController* playerController)
{
	if (IsValid(playerController->GetPawn()))
	{
		playerController->GetPawn()->Destroy();
	}


	ATankFighterCharacter *newPawn = GetWorld()->SpawnActor<ATankFighterCharacter>(tankClass, ChoosePlayerStart(playerController)->GetActorTransform());
	playerController->Possess(newPawn);
	playerController->playerID = GetNumPlayers() + 1;
}


