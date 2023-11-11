// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/ProjectileCharge.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "../TankFighterCharacter.h"


// Sets default values
AProjectileCharge::AProjectileCharge(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Pickup Collider"));
	niagaraSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pickup Niagara Effect"));

	sphereCollision->InitSphereRadius(100.0f);

	RootComponent = sphereCollision;
	niagaraSystem->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectileCharge::BeginPlay()
{
	Super::BeginPlay();
	chargeValue = FMath::FRandRange(1.0f,5.0f);
	sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectileCharge::OnOverlapBegin);
}

// Called every frame
void AProjectileCharge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileCharge::DeactivatePickup()
{
	sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	niagaraSystem->Deactivate();

	FTimerHandle reActivationTimer;
	GetWorld()->GetTimerManager().SetTimer(reActivationTimer, this, &AProjectileCharge::ActivatePickup, 3.0f, false, 3.0f);
}

void AProjectileCharge::ActivatePickup()
{
	niagaraSystem->Activate();
	sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	chargeValue = FMath::FRandRange(1.0f, 5.0f);
}



void AProjectileCharge::OnOverlapBegin(UPrimitiveComponent* newComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (OtherActor->GetClass()->IsChildOf(ATankFighterCharacter::StaticClass()))
		{
			Cast<ATankFighterCharacter>(OtherActor)->RechargeProjectiles(chargeValue);
			DeactivatePickup();
		}

	}
}

