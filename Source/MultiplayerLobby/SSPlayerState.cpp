// Fill out your copyright notice in the Description page of Project Settings.


#include "SSPlayerState.h"
#include "SSGameModeBase.h"
#include "Net/UnrealNetwork.h"

void ASSPlayerState::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//Replicate current health.
	DOREPLIFETIME(ASSPlayerState, kills);
	DOREPLIFETIME(ASSPlayerState, deaths);
	DOREPLIFETIME(ASSPlayerState, winner);
}

void ASSPlayerState::OnRep_Kills()
{
}

void ASSPlayerState::OnRep_Deaths()
{

}

void ASSPlayerState::OnRep_Winner()
{

}

void ASSPlayerState::AddKill()
{
	if (HasAuthority())
	{
		kills += 1;

		ASSGameModeBase* gameMode = (ASSGameModeBase*)GetWorld()->GetAuthGameMode();
		gameMode->CheckEndGame();
	}
}

void ASSPlayerState::RemoveKill()
{
	if (HasAuthority())
	{
		kills -= 1;
		kills = FMath::Max(kills, 0);

		ASSGameModeBase* gameMode = (ASSGameModeBase*)GetWorld()->GetAuthGameMode();
		//gameMode->CheckEndGame();
	}
}

void ASSPlayerState::AddDeath()
{
	if (HasAuthority())
	{
		deaths += 1;
	}
}