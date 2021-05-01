// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellProperties.h"
#include "SpellBook.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERLOBBY_API USpellBook : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellBook();

	void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void UpdateCooldowns(float deltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	FSpellProperties& GetSpellByName(FString name) 
	{ 
		return spellList[name];
	}

	///////////////////////
	// Variables
protected:

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Replicated, Category = "Spells")
	TMap<FString, FSpellProperties> spellList;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Replicated, Category = "Spells")
	FString primarySpellName;
};
