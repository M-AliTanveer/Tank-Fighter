// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "TankFighterCharacter.h"
#include "Blueprint/UserWidget.h"
#include "TankFighterPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class ATankFighterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATankFighterPlayerController();

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Movement Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SetDestinationClickAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ShootAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ScoreBoardToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> scoreboardWidgetClass;

	UPROPERTY()
	UUserWidget* scoreboardWidget;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int playerID;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay();

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();

	/** Input handler for fire action. */
	void AttemptProjectileFire();

	void ToggleScoreboard();

	virtual void OnPossess(APawn* InPawn) override;

private:
	FVector CachedDestination;

	UPROPERTY(Replicated)
	ATankFighterCharacter* posessedTank;


protected: //separately kept for organziation
	UFUNCTION(Server, unreliable)
	void Server_MoveToClick(ATankFighterPlayerController* playerController, FVector moveLocation);

	UFUNCTION(NetMulticast, unreliable)
	void MultiCast_MoveToClick(ATankFighterPlayerController* playerController, FVector moveLocation);
};


