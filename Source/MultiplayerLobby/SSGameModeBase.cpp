// Copyright Epic Games, Inc. All Rights Reserved.

#include "SSGameModeBase.h"
#include "TPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameMode.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GunGameState.h"
#include "SSPlayerState.h"
#include "Net/UnrealNetwork.h"

ASSGameModeBase::ASSGameModeBase()
{
	GameStateClass = AGunGameState::StaticClass();
	PlayerStateClass = ASSPlayerState::StaticClass();

	SpawnDelay = 5.0f;
	RemoveBodyDelay = 10.0f;
	ScoreGoal = 10;
}

void ASSGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* newPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(newPlayer);

	ATPCharacter* player = Cast<ATPCharacter>(newPlayer->GetCharacter());
}

void ASSGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	
	//(SSPlayerControllerBase*)Exiting->
}

void ASSGameModeBase::SpawnPlayer_Implementation(class APlayerController* playerController, TSubclassOf<ACharacter>  character)
{
	TArray<AActor*> spawnLocations;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), spawnLocations);

	AActor* selectedSpawnLocation = spawnLocations[rand() % spawnLocations.Num()];
	FActorSpawnParameters spawnParameters;

	ACharacter* newChar = GetWorld()->SpawnActor<ACharacter>(character, selectedSpawnLocation->GetTransform(), spawnParameters);

	playerController->Possess(newChar);
}

void ASSGameModeBase::UpdateChat_Implementation(const FText& sender, const FText& chatText)
{

}

void ASSGameModeBase::RespawnPlayer_Implementation(const ATPCharacter* player, TSubclassOf<ACharacter>  character)
{
	AController* controller = &(*player->GetController());
	player->GetController()->UnPossess();

	SpawnPlayer((APlayerController*)controller, character);
}


void ASSGameModeBase::CheckEndGame_Implementation()
{
	ATPCharacter* topScorer = GetTopScorer();
	int topScore = topScorer->GetPlayerState<ASSPlayerState>()->GetKills();

	GetGameState<AGunGameState>()->SetTopScore(topScore);
	GetGameState<AGunGameState>()->SetTopScorer(topScorer);

	// show score screen, remove damage from players.

	// display timer
	// set delay to close game
}

ATPCharacter* ASSGameModeBase::GetTopScorer()
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

