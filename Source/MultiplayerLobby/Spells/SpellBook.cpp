// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellBook.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"
#include "Spell.h"

// Sets default values for this component's properties
USpellBook::USpellBook()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

void USpellBook::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(USpellBook, spellList);
	//DOREPLIFETIME(USpellBook, primarySpellName);
}


// Called when the game starts
void USpellBook::BeginPlay()
{
	Super::BeginPlay();

	// Add Spells
	// Adding Basic Spell for now
	FSpellProperties magicMissile;
	magicMissile.damage = 10.0f;
	magicMissile.fireRate = 2.0f;
	magicMissile.initialSpeed = 2000.0f;
	magicMissile.maxSpeed = 3000.0f;
	magicMissile.name = "MagicMissile";
	magicMissile.acceleration = 10.0f;
	magicMissile.cooldownTimer = 0.0f;
	magicMissile.isChargable = false;
	magicMissile.range = 15000.0f;
	magicMissile.isHoming = true;
	magicMissile.lifeTime = 2.0f;
	magicMissile.hasExplosion = true;
	magicMissile.explosionProperties.acceleration = 1000.0f;
	magicMissile.explosionProperties.damageFalloffStart = 0.5f;
	magicMissile.explosionProperties.damageInterval = 100.0f;
	magicMissile.explosionProperties.initialGrowSpeed = 1000.0f;
	magicMissile.explosionProperties.maxGrowSpeed = 5000.0f;
	magicMissile.explosionProperties.maxDamage = 50.0f;
	magicMissile.explosionProperties.minDamage = 10.0f;
	magicMissile.explosionProperties.maxSize = 500.0f;

	spellList.Add(magicMissile.name, magicMissile);
}


// Called every frame
void USpellBook::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCooldowns(DeltaTime);
}

ASpell* USpellBook::GetSpellByName2(FString name)
{
	for (int i = 0; i < spells.Num(); ++i)
	{
		if (spells[i].GetDefaultObject()->GetProperties().name == name)
		{
			return spells[i].GetDefaultObject();
		}
	}
	return 0;
}

void USpellBook::UpdateCooldowns(float deltaTime)
{
	for (auto& spellProp : spellList)
	{
		if (spellProp.Value.cooldownTimer != 0.0f)
		{
			float cooldown = spellProp.Value.cooldownTimer;
			cooldown -= deltaTime * spellProp.Value.fireRate;
			spellProp.Value.cooldownTimer = FMath::Max(cooldown, 0.0f);
		}
	}

	for (int i = 0; i < spells.Num(); ++i)
	{
		ASpell* spell = spells[i].GetDefaultObject();
		if (spell->GetProperties().cooldownTimer != 0.0f)
		{
			float cooldown = spell->GetProperties().cooldownTimer;
			cooldown -= deltaTime * spell->GetProperties().fireRate;
			spell->GetProperties().cooldownTimer = FMath::Max(cooldown, 0.0f);
		}
	}
}
