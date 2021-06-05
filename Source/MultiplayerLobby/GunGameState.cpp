// Fill out your copyright notice in the Description page of Project Settings.


#include "GunGameState.h"
#include "Net/UnrealNetwork.h"

void AGunGameState::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(AGunGameState, topScore);
	DOREPLIFETIME(AGunGameState, topScorer);
}

void AGunGameState::OnRep_TopScore()
{
}

void AGunGameState::OnRep_TopScorer()
{

}

void AGunGameState::SetTopScore(int score)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		topScore = score;
	}
}

void AGunGameState::SetTopScorer(ATPCharacter* character)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		topScorer = character;
	}
}
