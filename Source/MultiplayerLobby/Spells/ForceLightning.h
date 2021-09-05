// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOESpell.h"
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

	virtual void SpellEnd() override;


protected:

	void SpawnSpellActor();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Variables
private:

protected:
	float chargeTime = 1.0f;
	const float chargeTimeTotal = 1.0f;
	
};
