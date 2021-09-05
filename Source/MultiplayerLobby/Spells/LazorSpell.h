// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "LazorSpell.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API ALazorSpell : public ASpell
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALazorSpell();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PrepareSpell(FVector target, const FSpellProperties& spellProps) override;

	virtual void Fire() override;

	virtual void FireAt(FVector target) override;

	virtual void OnImpact(UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void BeginCharge() override;

	virtual void Charging(float detalTime) override;

	virtual void EndCharge() override;

	virtual void SpellEnd() override;

protected:
	// Lazor spell specific
	virtual void AimLazor(float deltaTime);

	virtual void CheckTargetCooldowns(float deltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void PrepareLazor();

	UFUNCTION(Server, Reliable)
	void UpdateLazorServer(FVector position, FVector hitPoint, float scale, bool explosion);

	UFUNCTION(NetMulticast, Reliable)
	void UpdateLazor(FVector position, FVector hitPoint, float scale, bool explosion);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Variables
private:

protected:
	class UParticleSystem* explosionEffect;

	TMap<class ATPCharacter*, float> targets;

};
