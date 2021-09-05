// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellProperties.h"
#include "Spell.h"
#include "SpellBook.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERLOBBY_API USpellBook : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellBook();

	void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual void OnKill(int score);

	UFUNCTION(BlueprintCallable)
	ASpell* GetSpellByName(FString name);

	UFUNCTION(BlueprintCallable)
	virtual ASpell* GetPrimarySpell() { return 0; }

	UFUNCTION(BlueprintCallable)
	virtual ASpell* GetSecondarySpell() { return 0; }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int GetSpellCount() { return spells.Num(); }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void UpdateCooldowns(float deltaTime);

	virtual void UpdateSpreads(float deltaTime);

	///////////////////////
	// Variables
protected:

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Replicated, Category = "Spells")
	TArray<TSubclassOf<ASpell>> spells;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Replicated, Category = "Spells")
	FString primarySpellName;
};
