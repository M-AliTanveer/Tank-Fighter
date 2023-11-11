// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TankFighterPlayerController.h"
#include "TankFighterGameMode.generated.h"

class ATankFighterGameState;

UCLASS(minimalapi)
class ATankFighterGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ACharacter> tankClass;

	ATankFighterGameState* gameStateRef;

public:

	ATankFighterGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(Server, unreliable)
	void SpawnAndPosses(ATankFighterPlayerController* playerController);

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



