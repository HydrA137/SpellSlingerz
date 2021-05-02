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

	UFUNCTION(Server, Reliable)
	void RespawnPlayer(const ATPCharacter* player);

//////
// Variables

public:
	UPROPERTY(EditAnywhere)
	float SpawnDelay;

	UPROPERTY(EditAnywhere)
	float RemoveBodyDelay;
};



