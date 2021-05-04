// Copyright Epic Games, Inc. All Rights Reserved.

#include "MPGameMode.h"
#include "TPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameMode.h"
#include "GunGameState.h"
#include "SSPlayerState.h"
#include "Net/UnrealNetwork.h"

AMPGameMode::AMPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonBP/Blueprints/SpellCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	GameStateClass = AGunGameState::StaticClass();
	PlayerStateClass = ASSPlayerState::StaticClass();

	SpawnDelay = 5.0f;
	RemoveBodyDelay = 10.0f;
	ScoreGoal = 10;
}

void AMPGameMode::HandleStartingNewPlayer_Implementation(APlayerController* newPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(newPlayer);

	ATPCharacter* player = Cast<ATPCharacter>(newPlayer->GetCharacter());
}

void AMPGameMode::RespawnPlayer_Implementation(const ATPCharacter* player)
{
	AController* characterController = player->GetController();

	player->GetController()->UnPossess();

	RestartPlayer(characterController);
}


void AMPGameMode::CheckEndGame_Implementation()
{
	ATPCharacter* topScorer = GetTopScorer();
	int topScore = topScorer->GetPlayerState<ASSPlayerState>()->GetKills();

	GetGameState<AGunGameState>()->SetTopScore(topScore);
	GetGameState<AGunGameState>()->SetTopScorer(topScorer);

	// show score screen, remove damage from players.

	// display timer
	// set delay to close game
}

ATPCharacter* AMPGameMode::GetTopScorer()
{
	int highestScore = -1;
	ATPCharacter* topCharacter = 0;
	for (FConstPlayerControllerIterator iterator = GetWorld()->GetPlayerControllerIterator(); iterator; ++iterator)
	{
		int score = iterator->Get()->GetPlayerState<ASSPlayerState>()->GetKills();
		if (score > highestScore)
		{
			highestScore = score;
			topCharacter = Cast<ATPCharacter>(iterator->Get()->GetCharacter());
		}
	}

	return topCharacter;
}
