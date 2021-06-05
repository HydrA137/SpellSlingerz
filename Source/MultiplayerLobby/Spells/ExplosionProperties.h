// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExplosionProperties.generated.h"


USTRUCT(BlueprintType)
struct FExplosionProperties
{
	GENERATED_BODY();
	
	///////////////////
	// Size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	float maxGrowSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	float initialGrowSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	float acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Size")
	float maxSize;
	
	//////////////////
	// Offense
	//The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	TSubclassOf<class UDamageType> damageType;

	//The damage dealt by this projectile.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	float maxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	float minDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	float damageFalloffStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	float damageInterval;
};
