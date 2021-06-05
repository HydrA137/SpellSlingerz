// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPGameMode.generated.h"

UCLASS(minimalapi)
class AMPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMPGameMode();
	
	UFUNCTION()
	float GetSpawnDelay() { return SpawnDelay; }

	UFUNCTION()
	float GetRemoveBodyDelay() { return RemoveBodyDelay; }

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RespawnPlayer(const ATPCharacter* player);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CheckEndGame();
	
	UFUNCTION(BlueprintCallable)
	class ATPCharacter* GetTopScorer();

	UFUNCTION(BlueprintCallable)
	int GetScoreGoal() { return ScoreGoal; }

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

//////
// Variables

protected:
	UPROPERTY(EditAnywhere)
	float SpawnDelay;

	UPROPERTY(EditAnywhere)
	float RemoveBodyDelay;

	UPROPERTY(EditAnywhere)
	int ScoreGoal;

	
};



