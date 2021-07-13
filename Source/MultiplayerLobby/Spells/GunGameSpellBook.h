// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBook.h"
#include "GunGameSpellBook.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API UGunGameSpellBook : public USpellBook
{
	GENERATED_BODY()

public:
	UGunGameSpellBook();
	
	virtual void OnKill(int score) override;

	virtual ASpell* GetPrimarySpell() override;

	virtual ASpell* GetSecondarySpell() override;

protected:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// variables
private:
	int currentSpell;
};
