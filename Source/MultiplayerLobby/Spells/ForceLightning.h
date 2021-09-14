// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "ForceLightning.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API AForceLightning : public ASpell
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AForceLightning();

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

	virtual void AimLightning(float deltaTime);

	virtual void CheckTargetCooldowns(float deltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void KillParticles();

	UFUNCTION(NetMulticast, Reliable)
	void ServerFire();

	UFUNCTION(Server, Reliable)
	void UpdateLightningServer(FVector spawnPoint, FVector hitPoint, FVector scale);

	UFUNCTION(NetMulticast, Reliable)
	void UpdateLightning(FVector spawnPoint, FVector hitPoint, FVector scale);



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Variables
public:

	class UParticleSystemComponent* LightningParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystem* LightningParticle;


private:

protected:
	float chargeTime = 1.0f;
	const float chargeTimeTotal = 1.0f;

	TMap<class ATPCharacter*, float> targets;
	
};
