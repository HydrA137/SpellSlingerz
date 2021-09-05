// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API ASSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void AddKill();

	UFUNCTION(BlueprintCallable)
	void RemoveKill();

	UFUNCTION(BlueprintCallable)
	int GetKills() { return kills; }

	UFUNCTION(BlueprintCallable)
	void AddDeath();

	UFUNCTION(BlueprintCallable)
	int GetDeaths() { return deaths; }

	UFUNCTION(BlueprintCallable)
	bool GetWinner() { return winner; }
	void SetWinner(bool Winner) { winner = Winner; }
	
protected:
	UFUNCTION()
	void OnRep_Kills();

	UFUNCTION()
	void OnRep_Deaths();

	UFUNCTION()
	void OnRep_Winner();

	// variables
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Kills)
	int kills;

	UPROPERTY(ReplicatedUsing = OnRep_Deaths)
	int deaths;

	UPROPERTY(ReplicatedUsing = OnRep_Winner)
	bool winner;
	
};
