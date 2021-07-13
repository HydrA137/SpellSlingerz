// Fill out your copyright notice in the Description page of Project Settings.

#include "LazorSpell.h"
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

ALazorSpell::ALazorSpell()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/_main/Spells/Particles/Lazor_Beam_Hit_Effect.Lazor_Beam_Hit_Effect"));
	if (DefaultExplosionEffect.Succeeded())
	{
		explosionEffect = DefaultExplosionEffect.Object;
	}
}

void ALazorSpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ALazorSpell::PrepareSpell(FVector targetPoint, const FSpellProperties& spellProps)
{
	// targetPoint unused
	Super::PrepareSpell(targetPoint, spellProps);

	if (!staticMesh)
	{
		TArray<UStaticMeshComponent*> Components;
		this->GetComponents<UStaticMeshComponent>(Components);
		/*for (int32 i = 0; i < Components.Num(); i++)
		{
			UStaticMeshComponent* StaticMeshComponent = Components[i];
			UStaticMesh* StaticMesh = StaticMeshComponent->StaticMesh;
		}*/
		if (Components.Num() > 0)
		{
			staticMesh = Components[0];
		}
	}	
}

void ALazorSpell::Fire()
{


}

void ALazorSpell::FireAt(FVector targetPoint)
{

}

void ALazorSpell::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void ALazorSpell::BeginCharge()
{
}

void ALazorSpell::Charging(float detalTime)
{
}

void ALazorSpell::EndCharge()
{
}

void ALazorSpell::SpellEnd()
{
	Destroy();
}

void ALazorSpell::Tick(float DeltaTime)
{
	CheckTargetCooldowns(DeltaTime);
	AimLazor(DeltaTime);
}

void ALazorSpell::AimLazor(float deltaTime)
{
	FHitResult hitResult = dynamic_cast<ATPCharacter*>(GetOwner())->GetLookPoint(properties.range, 25.0f);
	FVector castPoint = dynamic_cast<ATPCharacter*>(GetOwner())->GetSpellCastPoint();

	float scale = 1.0f;
	if (hitResult.Distance > 0)
	{
		scale = hitResult.Distance / 100.0f;
		// Lazor_Beam_Hit_Effect
		
		if (explosionEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionEffect, hitResult.ImpactPoint, (castPoint - hitResult.TraceEnd).Rotation(), true, EPSCPoolMethod::AutoRelease);
		}

		ATPCharacter* targetPlayer = dynamic_cast<ATPCharacter*>(hitResult.Actor.Get());
		if (targetPlayer)
		{
			if (!targets.Contains(targetPlayer))
			{
				targets.Add(targetPlayer, 0.0f);
			}
			else if (*targets.Find(targetPlayer) <= 0.0f)
			{
				UGameplayStatics::ApplyDamage(targetPlayer, properties.damage, GetInstigator()->Controller, this, properties.damageType);
				*targets.Find(targetPlayer) = 1.0f;
			}
		}
	}
	else
	{
		scale = properties.range / 100.0f;
	}

	staticMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, scale));
	staticMesh->SetRelativeLocation(FVector(scale * 50.0f, 0.0f, 0.0f));

	FVector direction = hitResult.TraceEnd - castPoint;
	SetActorRotation(direction.Rotation());
	SetActorLocation(castPoint);
}

void ALazorSpell::CheckTargetCooldowns(float detlaTime)
{
	for (TPair<ATPCharacter*, float> pair : targets)
	{
		*targets.Find(pair.Key) = FMath::Max(pair.Value - detlaTime * properties.fireRate, 0.0f);
	}
}