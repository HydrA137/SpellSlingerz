// Fill out your copyright notice in the Description page of Project Settings.


#include "ForceLightning.h"
#include "../TPCharacter.h"
#include "Components/StaticMeshComponent.h"

AForceLightning::AForceLightning()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//AOE_TargettingMesh = CreateDefaultSubobject<UStaticMeshComponent>("AOE_TargettingMesh");
}



void AForceLightning::SpellEnd()
{
	//Spawn something to deal dmg
	isCharging = false;

	//SpawnSpellActor();

	Destroy();
}

void AForceLightning::SpawnSpellActor()
{
	if (HasAuthority())
	{
		////Copied Code to spawn actor
		//UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/_Main/Spells/Icicle")));

		//UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
		//if (!SpawnActor)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CANT FIND OBJECT TO SPAWN")));
		//	return;
		//}

		//UClass* SpawnClass = SpawnActor->StaticClass();
		//if (SpawnClass == NULL)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CLASS == NULL")));
		//	return;
		//}

		//UWorld* World = GetWorld();
		//FActorSpawnParameters SpawnParams;
		//SpawnParams.Owner = this->GetOwner();
		//SpawnParams.Instigator = this->GetInstigator();
		//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	}
	else
	{
		//Server_SpawnSpellActor();
	}
}

void AForceLightning::Tick(float deltaTime)
{
	FHitResult hit = dynamic_cast<ATPCharacter*>(GetOwner())->GetLookPoint(properties.range, 1.0f);
	SetActorLocation(hit.Location);
	SetActorRotation(FRotationMatrix::MakeFromZ(hit.Normal).Rotator());
}

