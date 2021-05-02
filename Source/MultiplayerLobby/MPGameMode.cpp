// Copyright Epic Games, Inc. All Rights Reserved.

#include "MPGameMode.h"
#include "TPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMPGameMode::AMPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonBP/Blueprints/SpellCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	SpawnDelay = 5.0f;
	RemoveBodyDelay = 10.0f;
}

void AMPGameMode::RespawnPlayer_Implementation(const ATPCharacter* player)
{
	AController* characterController = player->GetController();

	player->GetController()->UnPossess();

	RestartPlayer(characterController);
}
