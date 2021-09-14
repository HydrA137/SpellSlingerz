// Fill out your copyright notice in the Description page of Project Settings.


#include "ForceLightning.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "../TPCharacter.h"

AForceLightning::AForceLightning()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//AOE_TargettingMesh = CreateDefaultSubobject<UStaticMeshComponent>("AOE_TargettingMesh");
}
void AForceLightning::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AForceLightning::PrepareSpell(FVector targetPoint, const FSpellProperties& spellProps)
{
	// targetPoint unused
	Super::PrepareSpell(targetPoint, spellProps);

	PrepareLightning();

}

void AForceLightning::PrepareLightning_Implementation()
{
}

void AForceLightning::Fire()
{
	LightningParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LightningParticle, dynamic_cast<ATPCharacter*>(GetOwner())->GetSpellCastPoint(), FRotator::ZeroRotator, false);
}

void AForceLightning::FireAt(FVector targetPoint)
{

}

void AForceLightning::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AForceLightning::BeginCharge()
{
}

void AForceLightning::Charging(float detalTime)
{
}

void AForceLightning::EndCharge()
{
}

void AForceLightning::SpellEnd()
{
	LightningParticleComponent->DeactivateSystem();
	Destroy();
}

void AForceLightning::Tick(float DeltaTime)
{
	if (dynamic_cast<APawn*>(GetOwner())->IsLocallyControlled())
	{
		CheckTargetCooldowns(DeltaTime);
		AimLightning(DeltaTime);
	}
}

void AForceLightning::AimLightning(float deltaTime)
{
	TArray<AActor*> hitResult = dynamic_cast<ATPCharacter*>(GetOwner())->GetLookSphere(properties.range, properties.minAOERadius);
	FVector castPoint = dynamic_cast<ATPCharacter*>(GetOwner())->GetSpellCastPoint();
	FVector hitPoint = castPoint;
	FVector lightningThickness = { 0.5f, 0.5f, 0.5f };
	bool explosion = false;

	LightningParticleComponent->SetBeamSourcePoint(0, castPoint, 0);
	

	//Get all players within the area
	TArray<AActor*> hitPlayers;
	for (int i = 0; i < hitResult.Num(); ++i)
	{
		if (hitResult[i]->ActorHasTag("Player"))
		{
			hitPlayers.Add(hitResult[i]);			
		}
	}

	//Sort them by furthest to the player
	AActor* PlayerCaster = GetOwner();
	hitPlayers.Sort([PlayerCaster](const AActor& A, const AActor& B) {
		float DistA = A.GetDistanceTo(PlayerCaster);
		float DistB = B.GetDistanceTo(PlayerCaster);
		return DistA < DistB;
	});

	//Check to make sure nothing is in the way
	//First to return true is the clostest without obstructions
	for (int i = 0; i < hitPlayers.Num(); ++i)
	{
		FHitResult result;
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), castPoint, hitPlayers[i]->GetActorLocation(), 5.0f, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, { GetOwner() }, EDrawDebugTrace::None, result, true))
		{
			if (result.Actor->ActorHasTag("Player"))
			{
				lightningThickness = FVector{ 1.6f, 1.6f, 1.6f };
				hitPoint = hitPlayers[i]->GetActorLocation();
				
				LightningParticleComponent->SetBeamTargetPoint(0, hitPoint, 0);

				ATPCharacter* targetPlayer = dynamic_cast<ATPCharacter*>(hitPlayers[i]);
				if (!targets.Contains(targetPlayer))
				{
					targets.Add(targetPlayer, 0.0f);
				}
				else if (*targets.Find(targetPlayer) <= 0.0f)
				{
					UGameplayStatics::ApplyDamage(targetPlayer, properties.damage, GetInstigator()->Controller, this, properties.damageType);
					*targets.Find(targetPlayer) = 1.0f;
				}

				break;
			}
		}
	}

	
	LightningParticleComponent->SetVectorParameter("InitSize", lightningThickness);
	LightningParticleComponent->Activate();
	LightningParticleComponent->SetBeamTargetPoint(0, hitPoint, 0);


	float scale = 1.0f;

	FVector direction = hitPoint - castPoint;
	SetActorRotation(direction.Rotation());
	SetActorLocation(castPoint);

	if (GetLocalRole() == ROLE_Authority)
	{
		UpdateLightning(castPoint, hitPoint, scale, explosion);
	}
	else
	{
		UpdateLightningServer(castPoint, hitPoint, scale, explosion);
	}
}

void AForceLightning::UpdateLightningServer_Implementation(FVector position, FVector hitPoint, float scale, bool explosion)
{
	UpdateLightning(position, hitPoint, scale, explosion);
}

void AForceLightning::UpdateLightning_Implementation(FVector position, FVector hitPoint, float scale, bool explosion)
{
	if (!dynamic_cast<APawn*>(GetOwner())->IsLocallyControlled())
	{
		
	}
}

void AForceLightning::CheckTargetCooldowns(float detlaTime)
{
	for (TPair<ATPCharacter*, float> pair : targets)
	{
		*targets.Find(pair.Key) = FMath::Max(pair.Value - detlaTime * properties.fireRate, 0.0f);
	}
}

