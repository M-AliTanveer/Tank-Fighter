// Copyright Epic Games, Inc. All Rights Reserved.

#include "TankFighterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "TankFighter/TankProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"



ATankFighterCharacter::ATankFighterCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true; //Setting character to replicate saves us from using multicasts. Theoretically better in most cases.

}

void ATankFighterCharacter::BeginPlay()
{
	Super::BeginPlay();
	currentProjectileCharges = maxProjectileCharges;
}

void ATankFighterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATankFighterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATankFighterCharacter, currentProjectileCharges);
}

bool ATankFighterCharacter::CanFireProjectile()
{
	return currentProjectileCharges > 0;
}

void ATankFighterCharacter::FireProjectile()
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	if (GetMesh())
	{
		FTransform spawnTransform = GetMesh()->GetSocketTransform("GunEndSocket");
		currentProjectileCharges--;
		Server_FireProjectile(spawnTransform, controller);
	}
}

void ATankFighterCharacter::Server_FireProjectile_Implementation(FTransform spawnTransform, APlayerController* playerController)
{
	FActorSpawnParameters params;
	params.Owner = playerController;
	params.Instigator = this;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ATankProjectile>(projectileClass, spawnTransform, params);
	Mutlicast_PlayFireMontage();
}


void ATankFighterCharacter::RechargeProjectiles(int AddCharge)
{
	//will only be called on Server but as a precaution, we do an authority check
	if(HasAuthority()) 
		currentProjectileCharges = FMath::Clamp(currentProjectileCharges + AddCharge, 0, maxProjectileCharges);
}

void ATankFighterCharacter::Mutlicast_PlayFireMontage_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(fireMontage);
	}
}

void ATankFighterCharacter::Multicast_PlayDeathMontage_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(deathMontage);
	}
}

