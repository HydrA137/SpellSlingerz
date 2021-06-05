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

// Sets default values
ASpell::ASpell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(1.0f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SphereComponent->SetVisibility(true);
	RootComponent = SphereComponent;

	//Registering the Projectile Impact function on a Hit event.
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &ASpell::OnImpact);
	}

	//Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->InitialSpeed = 1000.0f;// properties.initialSpeed;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;// properties.maxSpeed;
	ProjectileMovementComponent->Velocity = FVector(0.0f, 0.0f, 0.0f);	

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		explosionEffect = DefaultExplosionEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultTravelEffect(TEXT("/Game/Particles/P_Sparkles.P_Sparkles"));
	if (DefaultTravelEffect.Succeeded())
	{
		travelEffect = DefaultTravelEffect.Object;
	}

	isTraveling = false;
}

void ASpell::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	//DOREPLIFETIME(ASpell, currentSpeed);
	DOREPLIFETIME(ASpell, isTraveling);
	DOREPLIFETIME(ASpell, currentVelocity);
}

void ASpell::SetTarget(AActor* _homingTarget)
{
	homingTarget = _homingTarget;
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
	target = _target;
	direction = _target - GetActorLocation();
	direction.Normalize();
	isTraveling = true;

	currentSpeed = properties.initialSpeed;

	currentVelocity = direction * currentSpeed;
	OnVelocityChanged();

	if (GetLocalRole() == ROLE_Authority)
	{
		FTimerHandle lifeTimerHandle;
		GetWorldTimerManager().SetTimer(lifeTimerHandle, this, &ASpell::Destroyed, properties.lifeTime, false);
		OnTravellingChanged();
	}
}

void ASpell::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, properties.damage, NormalImpulse, Hit, GetInstigator()->Controller, this, properties.damageType);
	}

	Destroy();
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

void ASpell::Move(float deltaTime)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		currentSpeed = FMath::Min(properties.maxSpeed, currentSpeed + (properties.acceleration * deltaTime));
		if (homingTarget)
		{
			FVector targetVelocity = (homingTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			targetVelocity += homingTarget->GetVelocity() * targetVelocity.Size() / currentSpeed;

			FVector newVelocity = ProjectileMovementComponent->Velocity + targetVelocity * currentSpeed * deltaTime;
			newVelocity = newVelocity.GetSafeNormal() * currentSpeed;
			currentVelocity = newVelocity;

			OnVelocityChanged();
		}
	}

	
	ProjectileMovementComponent->UpdateComponentVelocity();
}

void ASpell::OnTravellingChanged()
{
	if (isTraveling)
	{
		travelEffectComponent = UGameplayStatics::SpawnEmitterAttached(travelEffect, SphereComponent, FName(TEXT("P_Sparkle")), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	}
}

void ASpell::OnVelocityChanged()
{
	ProjectileMovementComponent->Velocity = currentVelocity;
}

void ASpell::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	if (travelEffectComponent)
	{
		travelEffectComponent->DeactivateSystem();
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, explosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

	Destroy();
}

void ASpell::SetCurrentSpeed(float speed)
{
	currentSpeed = speed;
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

	if (isCharging)
	{
		Charging(DeltaTime);
	}
	else if (isTraveling)
	{
		Move(DeltaTime);
	}
}

