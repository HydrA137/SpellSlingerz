// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spell.h"
#include "MagicMissile.generated.h"

UCLASS()
class MULTIPLAYERLOBBY_API AMagicMissile : public ASpell
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagicMissile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
