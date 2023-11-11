// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "../TankFighterPlayerController.h"

#include "TankFighterGameState.generated.h"

class ATankFighterGameMode;

USTRUCT(BlueprintType)
struct FScoreStruct
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	int playerID;
	UPROPERTY(BlueprintReadOnly)
	int playerScore;
	ATankFighterPlayerController* playerController;

	FScoreStruct(ATankFighterPlayerController* playerControllerInput, int playerScoreInput)
	{
		playerController = playerControllerInput;
		playerScore = playerScoreInput;
		playerID = playerControllerInput->playerID;

	}

	FScoreStruct()
	{

	}

};
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotifyScoreUpdate);

UCLASS()

class TANKFIGHTER_API ATankFighterGameState : public AGameState
{
	GENERATED_BODY()

protected:
	ATankFighterGameMode* gameMode;

	UPROPERTY(ReplicatedUsing = OnRep_ScoreUpdate, BlueprintReadOnly)
	TArray<FScoreStruct> scoreArray;

	UPROPERTY(BlueprintAssignable)
	FNotifyScoreUpdate scoreUpdateEvent;

public:

	virtual void BeginPlay() override;

	void InitiateDelayedRespawn(ATankFighterPlayerController* attackingController, ATankFighterPlayerController* playerController, float waitTime);

	void PlayerJoined(ATankFighterPlayerController* newPlayer);

	UFUNCTION()
	void OnRep_ScoreUpdate();
};
