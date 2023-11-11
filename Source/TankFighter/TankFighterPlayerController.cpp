// Copyright Epic Games, Inc. All Rights Reserved.

#include "TankFighterPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "TankFighterCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Navigation/PathFollowingComponent.h"



ATankFighterPlayerController::ATankFighterPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
}

void ATankFighterPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	scoreboardWidget = CreateWidget<UUserWidget>(GetWorld(), scoreboardWidgetClass);
	if (scoreboardWidget)
	{
		scoreboardWidget->SetVisibility(ESlateVisibility::Hidden);
		scoreboardWidget->AddToViewport();
	}
}


void ATankFighterPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ATankFighterPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ATankFighterPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ATankFighterPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ATankFighterPlayerController::OnSetDestinationReleased);


		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ATankFighterPlayerController::AttemptProjectileFire);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Canceled, this, &ATankFighterPlayerController::AttemptProjectileFire);

		EnhancedInputComponent->BindAction(ScoreBoardToggleAction, ETriggerEvent::Completed, this, &ATankFighterPlayerController::ToggleScoreboard);
		EnhancedInputComponent->BindAction(ScoreBoardToggleAction, ETriggerEvent::Canceled, this, &ATankFighterPlayerController::ToggleScoreboard);
	}
}

void ATankFighterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATankFighterPlayerController, posessedTank);
	DOREPLIFETIME(ATankFighterPlayerController, playerID);
}

void ATankFighterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	posessedTank = Cast<ATankFighterCharacter>(InPawn);
}

void ATankFighterPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ATankFighterPlayerController::OnSetDestinationTriggered()
{

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;

	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);


	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

}

void ATankFighterPlayerController::OnSetDestinationReleased()
{
	Server_MoveToClick(this, CachedDestination); //tell server to move all copies via multicast
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
}

void ATankFighterPlayerController::Server_MoveToClick_Implementation(ATankFighterPlayerController* playerController, FVector moveLocation)
{
	MultiCast_MoveToClick(playerController, moveLocation);
}

void ATankFighterPlayerController::MultiCast_MoveToClick_Implementation(ATankFighterPlayerController* playerController, FVector moveLocation)
{

	UPathFollowingComponent* PathFollowingComp = FindComponentByClass<UPathFollowingComponent>();
	if (PathFollowingComp == nullptr)
	{
		PathFollowingComp = NewObject<UPathFollowingComponent>(this);
		PathFollowingComp->RegisterComponentWithWorld(GetWorld());
		PathFollowingComp->Initialize();
	}

	if (!PathFollowingComp->IsPathFollowingAllowed())
	{
		PathFollowingComp->Initialize();
	}
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(playerController, moveLocation); //uses Pathfinding on NavMesh to go towards location. Utilizes Modified A*

}



void ATankFighterPlayerController::AttemptProjectileFire()
{
	if (posessedTank->CanFireProjectile())
	{
		posessedTank->FireProjectile();
	}
}

void ATankFighterPlayerController::ToggleScoreboard()
{
	if (scoreboardWidget)
	{
		if (scoreboardWidget->GetVisibility() == ESlateVisibility::Visible)
		{
			scoreboardWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			scoreboardWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
