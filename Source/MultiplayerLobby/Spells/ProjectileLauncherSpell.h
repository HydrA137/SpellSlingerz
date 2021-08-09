// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "ProjectileLauncherSpell.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API AProjectileLauncherSpell : public ASpell
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AProjectileLauncherSpell();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PrepareSpell(FVector target, const FSpellProperties& spellProps) override;
	
	virtual void Fire() override;

	virtual void FireAt(FVector target) override;

	virtual void OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void BeginCharge() override;

	virtual void Charging(float detalTime) override;

	virtual void EndCharge() override;

	virtual void SpellEnd() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	virtual void FireProjectiles(float DeltaTime);
	virtual void UpdateProjectileCooldowns(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void HandleFire(ASpell* spellTarget, FVector spawn, FVector _target, const FHitResult& hitResult);

	// Variables
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<class AProjectileSpell> projectile;

	bool stopFiring;
};
