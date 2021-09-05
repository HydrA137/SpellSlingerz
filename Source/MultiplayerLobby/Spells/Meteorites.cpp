// Fill out your copyright notice in the Description page of Project Settings.


#include "Meteorites.h"
#include "../TPCharacter.h"
#include "ProjectileLauncherSpell.h"

AMeteorites::AMeteorites()
{
	bReplicates = true;
	spawnHeight = 1000.0f;
}

void AMeteorites::Tick(float deltaTime)
{
	MoveMarker(deltaTime);
}

void AMeteorites::SpawnSpellActor()
{
	if (HasAuthority())
	{
		if (meteoriteLauncher)
		{
			AProjectileLauncherSpell* spell = meteoriteLauncher.GetDefaultObject();
			FActorSpawnParameters spawnParameters;
			spawnParameters.Instigator = GetInstigator();
			spawnParameters.Owner = GetOwner();
			float radius = FMath::RandRange(properties.minAOERadius, properties.maxAOERadius);
			FVector spawn = GetActorLocation() + FVector(FMath::RandRange(-radius, radius), FMath::RandRange(-radius, radius), spawnHeight);

			ASpell* activeSpell = GetWorld()->SpawnActor<ASpell>(spell->GetClass(), spawn, (spawn - GetActorLocation()).Rotation(), spawnParameters);
			activeSpell->PrepareSpell(GetActorLocation(), spell->GetProperties());
			activeSpell->Fire();
		}
	}
	else
	{
		Server_SpawnSpellActor();
	}
}
