// Fill out your copyright notice in the Description page of Project Settings.


#include "GunGameSpellBook.h"

UGunGameSpellBook::UGunGameSpellBook()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGunGameSpellBook::OnKill(int score)
{
	currentSpell += score;
}

ASpell* UGunGameSpellBook::GetPrimarySpell()
{
	return spells[currentSpell % spells.Num()].GetDefaultObject();
}

ASpell* UGunGameSpellBook::GetSecondarySpell()
{
	return nullptr;
}

void UGunGameSpellBook::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
