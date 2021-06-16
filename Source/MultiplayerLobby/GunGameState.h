// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GunGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERLOBBY_API AGunGameState : public AGameStateBase
{
	GENERATED_BODY()

public :

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SetTopScore(int score);
	
	UFUNCTION(BlueprintCallable)
	int GetTopScore() { return topScore; }

	UFUNCTION(BlueprintCallable)
	void SetTopScorer(class ATPCharacter* character);

	UFUNCTION(BlueprintCallable)
	class ATPCharacter* GetTopScorer() { return topScorer; }

protected:
	UFUNCTION()
	void OnRep_TopScore();

	UFUNCTION()
	void OnRep_TopScorer();
	
	// variables
protected:

	UPROPERTY(ReplicatedUsing = OnRep_TopScore)
	int topScore;

	UPROPERTY(ReplicatedUsing = OnRep_TopScorer)
	class ATPCharacter* topScorer;
};
