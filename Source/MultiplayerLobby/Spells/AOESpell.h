// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "AOESpell.generated.h"


/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API AAOESpell : public ASpell
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAOESpell();

	virtual void Fire() override;

	virtual void FireAt(FVector _target) override;

	virtual void BeginCharge() override;

	virtual void Charging(float detlaTime) override;

	virtual void EndCharge() override;

	virtual void SpellEnd() override;

	UPROPERTY(EditAnywhere, Category = "Components");
	class UStaticMeshComponent* AOE_TargettingMesh;

protected:

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Variables
private:

protected:
	float chargeTime = 1.0f;
	const float chargeTimeTotal = 1.0f;
	
};
