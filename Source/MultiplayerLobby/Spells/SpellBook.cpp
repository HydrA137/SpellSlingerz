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
}


// Called every frame
void USpellBook::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCooldowns(DeltaTime);
}

void USpellBook::OnKill(int score)
{

}

ASpell* USpellBook::GetSpellByName(FString name)
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
	for (int i = 0; i < spells.Num(); ++i)
	{
		ASpell* spell = spells[i].GetDefaultObject();
		float cooldown = spell->GetProperties().cooldownTimer;
		if (spell->GetProperties().cooldownTimer != 0.0f && !spell->GetProperties().isHeld)
		{
			cooldown -= deltaTime * spell->GetProperties().fireRate;
			spell->GetProperties().cooldownTimer = FMath::Max(cooldown, 0.0f);
		}
	}
}

void USpellBook::UpdateSpreads(float deltaTime)
{
	for (int i = 0; i < spells.Num(); ++i)
	{
		ASpell* spell = spells[i].GetDefaultObject();
		float spreadAngle = spell->GetProperties().spreadAngle;
		spreadAngle -= deltaTime;
		spell->GetProperties().spreadAngle = FMath::Max(spell->GetProperties().minSpreadAngle, spreadAngle);
	}
}

void USpellBook::SetSpell(int SpellNumber)
{

}
