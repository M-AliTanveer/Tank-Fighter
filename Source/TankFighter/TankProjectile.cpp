// Fill out your copyright notice in the Description page of Project Settings.

#include "TankProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TankFighterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TankFighterGameMode.h"
#include "TankFighterGameState.h"




// Sets default values
ATankProjectile::ATankProjectile(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; //ensure spawning across all devies. 

	projectileBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Body"));
	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	capsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collision"));

	RootComponent = capsuleCollision;
	projectileBody->SetupAttachment(RootComponent);

	projectileMovementComponent->MaxSpeed = 1500.0f;
	projectileMovementComponent->InitialSpeed = 1500.0f;
	projectileMovementComponent->ProjectileGravityScale = 0.0f;

	capsuleCollision->InitCapsuleSize(10.0f, 10.0f);
	InitialLifeSpan = 5.0f;
}

// Called when the game starts or when spawned
void ATankProjectile::BeginPlay()
{
	Super::BeginPlay();
	capsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ATankProjectile::OnOverlapBegin);
}

// Called every frame
void ATankProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATankProjectile::OnOverlapBegin(UPrimitiveComponent* newComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (OtherActor == this)
			return;
		if (OtherActor->GetClass()->IsChildOf(ATankFighterCharacter::StaticClass()))
		{
			if (OtherActor != GetInstigator())
			{
				ATankFighterCharacter* opponent = Cast<ATankFighterCharacter>(OtherActor);
				ATankFighterGameState *gameState = Cast<ATankFighterGameState>(UGameplayStatics::GetGameState(GetWorld()));
				ATankFighterPlayerController* opponentController = Cast<ATankFighterPlayerController>(opponent->GetController());
				ATankFighterPlayerController* attackingController = Cast<ATankFighterPlayerController>(GetOwner());

				float montageLength = opponent->deathMontage->CalculateSequenceLength();
					
				opponent->Multicast_PlayDeathMontage();

				gameState->InitiateDelayedRespawn(attackingController,opponentController,montageLength+1);
				Destroy();

			}
		}
		else
		{
			Destroy();
		}
	}
}
