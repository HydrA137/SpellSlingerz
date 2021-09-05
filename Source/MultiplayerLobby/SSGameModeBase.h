// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SSGameModeBase.generated.h"

UCLASS(minimalapi)
class ASSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASSGameModeBase();
	
	UFUNCTION()
	float GetSpawnDelay() { return SpawnDelay; }

	UFUNCTION()
	float GetRemoveBodyDelay() { return RemoveBodyDelay; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SpawnPlayer(class APlayerController* playerController, TSubclassOf<class ACharacter> character);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateChat(const FText& sender, const FText& chatText);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RespawnPlayer(const ATPCharacter* player, TSubclassOf<ACharacter>  character);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CheckEndGame();
	
	UFUNCTION(BlueprintCallable)
	class ATPCharacter* GetTopScorer();

	UFUNCTION(BlueprintCallable)
	int GetScoreGoal() { return ScoreGoal; }

	UFUNCTION(BlueprintImplementableEvent)
	void GameFinished();

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting);

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



