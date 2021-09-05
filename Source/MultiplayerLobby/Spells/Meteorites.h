// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOESpell.h"
#include "Meteorites.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API AMeteorites : public AAOESpell
{
	GENERATED_BODY()

	// fuctions
public:
	// Sets default values for this actor's properties
	AMeteorites();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SpawnSpellActor() override;

	//variables

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Properties")
	TSubclassOf<class AProjectileLauncherSpell> meteoriteLauncher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Properties")
	float spawnHeight;
};
