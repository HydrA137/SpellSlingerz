// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "SpellProperties.h"
#include "ExplosionProperties.h"
#include "Explosion.h"
#include "Engine/World.h"

// Sets default values
ASpell::ASpell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	properties.cooldownTimer = 0.0f;
}

void ASpell::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASpell::PrepareSpell(FVector _target, const FSpellProperties& spellProps)
{
	target = _target;
	properties = spellProps;
}

void ASpell::Fire()
{
	FireAt(target);
}

void ASpell::FireAt(FVector _target)
{
}

void ASpell::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, properties.damage, NormalImpulse, Hit, GetInstigator()->Controller, this, properties.damageType);
	}

	SpellEnd();
}

void ASpell::BeginCharge()
{
	isCharging = true;
}

void ASpell::Charging(float deltaTime)
{
}

void ASpell::EndCharge()
{
	FireAt(target);
}


void ASpell::HandleExplosion_Implementation(const FExplosionProperties& explosionProps)
{
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParameters.Owner = this->GetOwner();

	if (explosionClass.GetDefaultObject())
	{
		AExplosion* explosion = GetWorld()->SpawnActor<AExplosion>(explosionClass, this->GetActorLocation(), this->GetActorRotation(), spawnParameters);
		if (explosion)
		{
			explosion->SetProperties(explosionClass.GetDefaultObject()->GetProperties());
			explosion->Explode();
		}
	}
}

void ASpell::SpellEnd()
{
	Destroy();
}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

