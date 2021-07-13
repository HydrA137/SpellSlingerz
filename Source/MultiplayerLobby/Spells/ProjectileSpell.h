// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellProperties.h"
#include "Explosion.h"
#include "Spell.h"
#include "ProjectileSpell.generated.h"

UCLASS()
class MULTIPLAYERLOBBY_API AProjectileSpell : public ASpell
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileSpell();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PrepareSpell(FVector target, const FSpellProperties& spellProps) override;

	virtual void Fire() override;

	virtual void FireAt(FVector target) override;

	virtual void OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void BeginCharge() override;

	virtual void Charging(float detalTime) override;

	virtual void EndCharge() override;

	virtual void SetHomingTarget(AActor* target);

	virtual void SpellEnd() override;

	virtual void Move(float deltaTime);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetCurrentSpeed(float speed) { currentSpeed = speed; }

	UFUNCTION()
	float GetCurrentSpeed() { return currentSpeed; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** RepNotify for changes made to current health.*/
	UFUNCTION()
	void OnTravellingChanged();

	UFUNCTION()
	void OnVelocityChanged();



	// Variable
protected:
	////////////////////////////////////////////////
	// Sphere component used to test collision.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* SphereComponent;

	// Movement component for handling projectile movement.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Particle system for the visuals of the spell
	class UParticleSystemComponent* travelEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystem* travelEffect;

	UPROPERTY(ReplicatedUsing = OnTravellingChanged)
	bool isTraveling;

	class AActor* homingTarget;

	////////////////////////////////////////////////
	// Movement/Targeting
	FVector direction;

	UPROPERTY(ReplicatedUsing = OnVelocityChanged)
	FVector currentVelocity;

	float currentSpeed;


};
