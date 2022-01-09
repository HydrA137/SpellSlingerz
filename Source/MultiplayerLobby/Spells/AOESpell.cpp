// Fill out your copyright notice in the Description page of Project Settings.


#include "AOESpell.h"
#include "../TPCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AAOESpell::AAOESpell()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	AOE_TargettingMesh = CreateDefaultSubobject<UStaticMeshComponent>("AOE_TargettingMesh");

	static ConstructorHelpers::FObjectFinder<UClass> ItemBlueprint(TEXT("Class'/Game/_Main/Spells/Icicle.Icicle_C'"));
	if (ItemBlueprint.Object) {
		MyItemBlueprint = (UClass*)ItemBlueprint.Object;
	}
}


void AAOESpell::Fire()
{

}

void AAOESpell::FireAt(FVector _target)
{
	//Make sure it only collides with static meshs
	//Dont want to spawn a icicle on a player

}


void AAOESpell::BeginCharge()
{
	//Load targetting reticle mesh
	isCharging = true;

	//Reset Charge Time
	chargeTime = chargeTimeTotal;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::SanitizeFloat(chargeTime));

}

void AAOESpell::Charging(float deltaTime)
{
	//Show targetting reticle, Slowly increasing in size.

	if (chargeTime <= 0.0f)
	{
		//Change reticle to "charged" version
	}
}

void AAOESpell::EndCharge()
{
	//Fire spell
	
}

void AAOESpell::SpellEnd()
{
	//Spawn something to deal dmg
	isCharging = false;

	SpawnSpellActor();

	Destroy();
}

bool AAOESpell::Server_SpawnSpellActor_Validate()
{
	return true;
}

void AAOESpell::Server_SpawnSpellActor_Implementation()
{
	SpawnSpellActor();
}

void AAOESpell::SpawnSpellActor()
{
	if (HasAuthority())
	{
		//Copied Code to spawn actor
		//UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/_Main/Spells/Icicle")));

		//UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
		/*if (!ItemBlueprint)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CANT FIND OBJECT TO SPAWN")));
			return;
		}

		UClass* SpawnClass = SpawnActor->StaticClass();
		if (SpawnClass == NULL)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CLASS == NULL")));
			return;
		}*/

		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Instigator = this->GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<AActor>(MyItemBlueprint, GetActorLocation(), GetActorRotation(), SpawnParams);
	}
	else
	{
		Server_SpawnSpellActor();
	}
}

void AAOESpell::Tick(float deltaTime)
{
	MoveMarker(deltaTime);
}

void AAOESpell::MoveMarker(float deltaTime)
{
	FHitResult hit = dynamic_cast<ATPCharacter*>(GetOwner())->GetLookPoint(properties.range, 1.0f);
	if (hit.Distance > 0)
	{
		SetActorLocation(hit.Location);
	}
	else
	{
		TArray<AActor*> toIgnore = { this, GetOwner() };
		FHitResult result;
		FVector start(hit.TraceEnd);
		FVector end = start + (FVector(0.0f,0.0f,-1.0f) * 3000.0f);
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, toIgnore, EDrawDebugTrace::None, result, true))
		{
			SetActorLocation(result.Location);
		}
	}

	SetActorRotation(FRotationMatrix::MakeFromZ(hit.Normal).Rotator());
}