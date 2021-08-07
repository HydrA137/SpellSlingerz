// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileLauncherSpell.h"
#include "ProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../TPCharacter.h"
#include "GameFramework/Actor.h"

AProjectileLauncherSpell::AProjectileLauncherSpell()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	properties.cooldownTimer = 0.0f;
	stopFiring = false;
}

void AProjectileLauncherSpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AProjectileLauncherSpell::PrepareSpell(FVector _target, const FSpellProperties& spellProps)
{
}

void AProjectileLauncherSpell::Fire()
{
}

void AProjectileLauncherSpell::FireAt(FVector _target)
{
}

void AProjectileLauncherSpell::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AProjectileLauncherSpell::BeginCharge()
{
}

void AProjectileLauncherSpell::Charging(float deltaTime)
{
}

void AProjectileLauncherSpell::EndCharge()
{
}

void AProjectileLauncherSpell::SpellEnd()
{
	Destroy();
	stopFiring = true;
}

void AProjectileLauncherSpell::Tick(float deltaTime)
{
	if (!stopFiring)
	{
		SetActorLocation(dynamic_cast<ATPCharacter*>(GetOwner())->GetSpellCastPoint());
		FireProjectiles(deltaTime);
		UpdateProjectileCooldowns(deltaTime);
	}
}

void AProjectileLauncherSpell::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileLauncherSpell::FireProjectiles(float deltaTime)
{
	AProjectileSpell* spell = projectile.GetDefaultObject();
	if (spell->IsReady())
	{
		FVector spawnLocation = GetActorLocation();

		FHitResult hitResult = dynamic_cast<ATPCharacter*>(GetOwner())->GetLookPoint(1000.0f, 5.0f);

		HandleFire(spell, spawnLocation, hitResult.TraceEnd, hitResult);
	}
}

void AProjectileLauncherSpell::UpdateProjectileCooldowns(float deltaTime)
{
	AProjectileSpell* spell = projectile.GetDefaultObject();
	if (spell->GetProperties().cooldownTimer != 0.0f && !spell->GetProperties().isHeld)
	{
		float cooldown = spell->GetProperties().cooldownTimer;
		cooldown -= deltaTime * spell->GetProperties().fireRate;
		spell->GetProperties().cooldownTimer = FMath::Max(cooldown, 0.0f);
	}
}

void AProjectileLauncherSpell::HandleFire_Implementation(ASpell* spellTarget, FVector spawn, FVector _target, const FHitResult& hitResult)
{
	FRotator spawnRotation = (_target - spawn).Rotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = GetOwner();

	int count = FMath::Max(1, FMath::RandRange(properties.minProjectileCount, properties.maxProjectileCount));
	// Get Spell based on type
	for (int i = 0; i < count; ++i)
	{
		ASpell* activeSpell = GetWorld()->SpawnActor<ASpell>(spellTarget->GetClass(), spawn, spawnRotation, spawnParameters);
		activeSpell->PrepareSpell(_target, spellTarget->GetProperties());

		if (activeSpell->GetProperties().isHoming && hitResult.Actor.IsValid())
		{
			ATPCharacter* targetPlayer = dynamic_cast<ATPCharacter*>(hitResult.Actor.Get());
			if (targetPlayer)
			{
				dynamic_cast<AProjectileSpell*>(activeSpell)->SetHomingTarget(hitResult.Actor.Get());
			}
		}

		activeSpell->Fire();
	}
	projectile.GetDefaultObject()->Fired();
}
