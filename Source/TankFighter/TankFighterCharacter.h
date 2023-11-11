// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TankFighterCharacter.generated.h"

class ATankProjectile;

UCLASS(Blueprintable)
class ATankFighterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATankFighterCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ATankProjectile> projectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* deathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* fireMontage; 

	UFUNCTION()
	void FireProjectile();

	UFUNCTION()
	bool CanFireProjectile();

	UFUNCTION()
	void RechargeProjectiles(int AddCharge);

	UFUNCTION(NetMulticast, unreliable)
	void Mutlicast_PlayFireMontage();

	UFUNCTION(NetMulticast, unreliable)
	void Multicast_PlayDeathMontage();

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Projectile charges available **/
	UPROPERTY(EditAnywhere, Category = "Tank Shooting")
	int maxProjectileCharges = 5;
	UPROPERTY(Replicated)
	int currentProjectileCharges;

protected:

	UFUNCTION(Server, unreliable)
	void Server_FireProjectile(FTransform spawnTransform, APlayerController *playerController);



};

