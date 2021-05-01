// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	
	///////////////////
	// Basic
	UPROPERTY()
	FString name;

	UPROPERTY()
	TEnumAsByte<ESpellType> spellType;

	UPROPERTY()
	bool isHoming;

	///////////////////
	// Movement
	UPROPERTY()
	float maxSpeed;

	UPROPERTY()
	float initialSpeed;

	UPROPERTY()
	float acceleration;

	UPROPERTY()
	float range;

	//////////////////
	// Offense
	//The damage type and damage that will be done by this projectile
	UPROPERTY()
	TSubclassOf<class UDamageType> damageType;

	//The damage dealt by this projectile.
	UPROPERTY()
	float damage;

	UPROPERTY()
	float fireRate;

	UPROPERTY()
	bool isChargable;

	UPROPERTY()
	float cooldownTimer = 0.0f;
};
