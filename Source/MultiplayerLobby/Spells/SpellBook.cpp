// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellBook.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"

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

	spellList.Add(magicMissile.name, magicMissile);
}


// Called every frame
void USpellBook::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCooldowns(DeltaTime);
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
}
