// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExplosionProperties.h"
#include "SpellProperties.generated.h"

UENUM(BlueprintType)
enum ESpellType
{
	ST_BASIC
};

USTRUCT(BlueprintType)
struct FSpellProperties
{
	GENERATED_BODY();
	
	bool IsReady() 
	{ 
		return cooldownTimer <= 0.0f; 
	}

	void Fired()
	{
		cooldownTimer = 1.0f;
	}

	void Reset()
	{
		cooldownTimer = 0.0f;
	}
	
	///////////////////
	// Basic
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
	TEnumAsByte<ESpellType> spellType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
	bool isHoming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
	bool isHeld;

	///////////////////
	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float maxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float initialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float lifeTime; //seconds

	//////////////////
	// Offense
	//The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	TSubclassOf<class UDamageType> damageType;

	//The damage dealt by this projectile.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	float damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	float fireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	bool isChargable;

	UPROPERTY(BlueprintReadOnly)
	float cooldownTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	bool hasExplosion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Offense")
	FExplosionProperties explosionProperties;
};
