// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSpell.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "SpellProperties.h"
#include "ExplosionProperties.h"
#include "Explosion.h"
#include "Engine/World.h"

// Sets default values
AProjectileSpell::AProjectileSpell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	if (!SphereComponent)
	{
		SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
		SphereComponent->InitSphereRadius(1.0f);
		SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		SphereComponent->SetVisibility(true);
		RootComponent = SphereComponent;

		//Registering the Projectile Impact function on a Hit event.
		if (GetLocalRole() == ROLE_Authority)
		{
			SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSpell::OnBeginOverlap);
			SphereComponent->OnComponentHit.AddDynamic(this, &ASpell::OnImpact);
		}
	}	

	//Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 200.0f;// properties.initialSpeed;
	ProjectileMovementComponent->MaxSpeed = 300.0f;// properties.maxSpeed;
	ProjectileMovementComponent->Velocity = FVector(0.0f, 0.0f, 0.0f);	
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		explosionEffect = DefaultExplosionEffect.Object;
	}*/
	// TODO: set new explosion effect

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultTravelEffect(TEXT("/Game/Particles/P_Sparkles.P_Sparkles"));
	if (DefaultTravelEffect.Succeeded())
	{
		travelEffect = DefaultTravelEffect.Object;
	}

	isTraveling = false;
	properties.cooldownTimer = 0.0f;
	destroyOnImpact = true;
}

void AProjectileSpell::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	//DOREPLIFETIME(ASpell, currentSpeed);
	DOREPLIFETIME(AProjectileSpell, isTraveling);
}

void AProjectileSpell::SetHomingTarget(AActor* _homingTarget)
{
	homingTarget = _homingTarget;
}

void AProjectileSpell::PrepareSpell(FVector _target, const FSpellProperties& spellProps)
{
	Super::PrepareSpell(_target, spellProps);
	if (properties.minGravWeight != properties.maxGravWeight)
	{
		ProjectileMovementComponent->ProjectileGravityScale = FMath::RandRange(properties.minGravWeight, properties.maxGravWeight);
	}
	else
	{
		ProjectileMovementComponent->ProjectileGravityScale = properties.minGravWeight;
	}
}

void AProjectileSpell::Fire()
{
	FireAt(target);
}

void AProjectileSpell::FireAt(FVector _target)
{
	target = _target;
	FVector temp = GetActorTransform().TransformVector(properties.firingDirection);
	direction = AddSpread(temp != FVector::ZeroVector ? temp : _target - GetActorLocation());
	direction.Normalize();
	isTraveling = true;

	currentSpeed = properties.initialSpeed;

	currentVelocity = direction * currentSpeed;
	OnVelocityChanged();

	if (GetLocalRole() == ROLE_Authority)
	{
		FTimerHandle lifeTimerHandle;
		float lifeTime = FMath::RandRange(properties.minLifeTime, properties.maxLifeTime);
		GetWorldTimerManager().SetTimer(lifeTimerHandle, this, &ASpell::SpellEnd, lifeTime, false);
		OnTravellingChanged();
	}
}

FVector AProjectileSpell::AddSpread(FVector curDirection)
{
	return UKismetMathLibrary::RandomUnitVectorInConeInDegrees(curDirection, GetProperties().spreadAngle);
}

void AProjectileSpell::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	SpellEnd();
}

void AProjectileSpell::OnBeginOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor &&
		OtherActor != GetOwner() &&
		OtherActor->GetOwner() != GetOwner())
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, properties.damage, OtherActor->GetActorLocation(), SweepResult, GetInstigator()->Controller, this, properties.damageType);

		SpellEnd();
	}
	else if (!OtherActor)
	{
		SpellEnd();
	}
}

void AProjectileSpell::BeginCharge()
{
	isCharging = true;
}

void AProjectileSpell::Charging(float deltaTime)
{
}

void AProjectileSpell::EndCharge()
{
	FireAt(target);
}

void AProjectileSpell::Move(float deltaTime)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		float distance = (target - GetActorLocation()).Size();
		if (distance <= 30.0f)
		{
			SpellEnd();
			return;
		}

		currentSpeed = FMath::Min(properties.maxSpeed, currentSpeed + (properties.acceleration * deltaTime));
		if (properties.isHoming)
		{
			FVector flyTo = homingTarget ? homingTarget->GetActorLocation() : target;
			FVector flyToVelocity = homingTarget ? homingTarget->GetVelocity() : FVector::ZeroVector;
			FVector targetVelocity = (flyTo - GetActorLocation()).GetSafeNormal();
			targetVelocity += flyToVelocity * targetVelocity.Size() / currentSpeed;

			FVector newVelocity = ProjectileMovementComponent->Velocity + targetVelocity * currentSpeed * properties.homingStrength * deltaTime;
			newVelocity = newVelocity.GetSafeNormal() * currentSpeed;
			currentVelocity = newVelocity;
		}
		else
		{
			FVector newVelocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * currentSpeed;
			currentVelocity = newVelocity;
		}

		OnVelocityChanged();

		ProjectileMovementComponent->UpdateComponentVelocity();
	}
}

void AProjectileSpell::OnTravellingChanged()
{
	if (isTraveling)
	{
		travelEffectComponent = UGameplayStatics::SpawnEmitterAttached(travelEffect, SphereComponent, FName(TEXT("P_Sparkle")), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	}
}

void AProjectileSpell::OnVelocityChanged()
{
	ProjectileMovementComponent->Velocity = currentVelocity;
}

void AProjectileSpell::SpellEnd()
{
	FVector spawnLocation = GetActorLocation();
	if (travelEffectComponent)
	{
		travelEffectComponent->DeactivateSystem();		
	}

	if (properties.hasExplosion) // Has an explosion
	{
		HandleExplosion(properties.explosionProperties);
	}
	
	if (destroyOnImpact)
	{
		ProjectileMovementComponent->Deactivate();
		SphereComponent->Deactivate();
		FTimerHandle deathTimerHandle;
		Super::SpellEnd();
	}
}

// Called when the game starts or when spawned
void AProjectileSpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileSpell::Tick(float DeltaTime)
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

