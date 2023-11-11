// Fill out your copyright notice in the Description page of Project Settings.


#include "TankFighterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "../TankFighterGameMode.h"
#include "Net/UnrealNetwork.h"


void ATankFighterGameState::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<ATankFighterGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ATankFighterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Here we list the variables we want to replicate
	DOREPLIFETIME(ATankFighterGameState, scoreArray);
}

void ATankFighterGameState::InitiateDelayedRespawn(ATankFighterPlayerController* attackingController, ATankFighterPlayerController* playerController, float waitTime)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(gameMode, &ATankFighterGameMode::SpawnAndPosses, playerController), waitTime, false);

	if (attackingController)
	{
		for (FScoreStruct& scoreStruct : scoreArray)
		{
			if (scoreStruct.playerController == attackingController)
			{
				scoreStruct.playerScore++;
				break;
			}
		}
		OnRep_ScoreUpdate(); //default behavious in C++ is to only call repNotify events on Clients. Since we are on listen server architecture, we will also call it manually once for the listen server.
	}
}

void ATankFighterGameState::PlayerJoined(ATankFighterPlayerController* newPlayer)
{
	if (newPlayer)
	{
		scoreArray.Add(FScoreStruct{ newPlayer , 0 });
	}
	OnRep_ScoreUpdate();
}

void ATankFighterGameState::OnRep_ScoreUpdate()
{
	scoreUpdateEvent.Broadcast();
}

