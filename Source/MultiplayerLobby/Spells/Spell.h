// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellProperties.h"
#include "Spell.generated.h"

UCLASS()
class MULTIPLAYERLOBBY_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpell();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category = "Spell_Events")
	virtual void PrepareSpell(FVector target, const FSpellProperties& spellProps);

	UFUNCTION(Category = "Spell_Events")
	virtual void Fire();

	UFUNCTION(Category = "Spell_Events")
	virtual void FireAt(FVector target);

	UFUNCTION(Category = "Spell_Events")
	virtual void OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Category = "Spell_Events")
	virtual void BeginCharge();

	virtual void Charging(float detalTime);

	UFUNCTION(Category = "Spell_Events")
	virtual void EndCharge();

	virtual void Move(float deltaTime);

	virtual void SetTarget(AActor* target);

	UFUNCTION()
	virtual void Destroyed() override;

	UFUNCTION()
	bool IsCharging() { return isCharging; }

	UFUNCTION()
	void SetCurrentSpeed(float speed);

	UFUNCTION()
	float GetCurrentSpeed() { return currentSpeed; }

	UFUNCTION()
	const FSpellProperties& GetProperties() { return properties; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** RepNotify for changes made to current health.*/
	UFUNCTION()
	void OnTravellingChanged();

	UFUNCTION()
	void OnVelocityChanged();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Variable
protected:
	////////////////////////////////////////////////
	// Visuals
	// Sphere component used to test collision.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComponent;

	// Static Mesh used to provide a visual representation of the object.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* staticMesh;

	// Movement component for handling projectile movement.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Particle system for the visuals of the spell
	class UParticleSystemComponent* travelEffectComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* travelEffect;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* explosionEffect;

	UPROPERTY(ReplicatedUsing = OnTravellingChanged)
	bool isTraveling;

	class AActor* homingTarget;

	////////////////////////////////////////////////
	// Movement/Targeting
	FVector target;
	FVector direction;

	UPROPERTY(ReplicatedUsing = OnVelocityChanged)
	FVector currentVelocity;

	float currentSpeed;

	////////////////////////////////////////////////
	// Properties
	// Spell Properties component 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	FSpellProperties properties;
	
	/////////////////////////////////////////////////
	// States
	bool isCharging;
};