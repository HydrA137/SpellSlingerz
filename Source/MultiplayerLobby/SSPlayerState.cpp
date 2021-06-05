// Fill out your copyright notice in the Description page of Project Settings.


#include "SSPlayerState.h"
#include "MPGameMode.h"
#include "Net/UnrealNetwork.h"

void ASSPlayerState::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//Replicate current health.
	DOREPLIFETIME(ASSPlayerState, kills);
	DOREPLIFETIME(ASSPlayerState, deaths);
}

void ASSPlayerState::OnRep_Kills()
{
}

void ASSPlayerState::OnRep_Deaths()
{

}

void ASSPlayerState::AddKill()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		kills += 1;

		AMPGameMode* gameMode = (AMPGameMode*)GetWorld()->GetAuthGameMode();
		gameMode->CheckEndGame();
	}
}

void ASSPlayerState::RemoveKill()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		kills += 1;

		AMPGameMode* gameMode = (AMPGameMode*)GetWorld()->GetAuthGameMode();
		//gameMode->CheckEndGame();
	}
}

void ASSPlayerState::AddDeath()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		deaths += 1;
	}
}