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
			SphereComponent->OnComponentHit.AddDynamic(this, &ASpell::OnImpact);
		}
	}	

	//Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->InitialSpeed = 1000.0f;// properties.initialSpeed;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;// properties.maxSpeed;
	ProjectileMovementComponent->Velocity = FVector(0.0f, 0.0f, 0.0f);	

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
}

void AProjectileSpell::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	//DOREPLIFETIME(ASpell, currentSpeed);
	DOREPLIFETIME(AProjectileSpell, isTraveling);
	DOREPLIFETIME(AProjectileSpell, currentVelocity);
}

void AProjectileSpell::SetHomingTarget(AActor* _homingTarget)
{
	homingTarget = _homingTarget;
}

void AProjectileSpell::PrepareSpell(FVector _target, const FSpellProperties& spellProps)
{
	Super::PrepareSpell(_target, spellProps);
	ProjectileMovementComponent->ProjectileGravityScale = FMath::RandRange(properties.minGravWeight, properties.maxGravWeight);
}

void AProjectileSpell::Fire()
{
	FireAt(target);
}

void AProjectileSpell::FireAt(FVector _target)
{
	target = _target;
	direction = AddSpread(_target - GetActorLocation());
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
	float upDown = FMath::RandRange(-GetProperties().spreadAngle, GetProperties().spreadAngle);
	float leftRight = FMath::RandRange(-GetProperties().spreadAngle, GetProperties().spreadAngle);
	FVector2D spreadArea = FVector2D(leftRight * 10.0f, upDown * 10.0f);
	spreadArea.Normalize();
	spreadArea *= FVector2D(leftRight, upDown);

	FRotator newRotator = FRotator(GetActorRotation().Pitch + upDown, GetActorRotation().Yaw + leftRight, GetActorRotation().Roll);

	SetActorRotation(newRotator);

	curDirection = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);

	return curDirection;
}

void AProjectileSpell::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, properties.damage, NormalImpulse, Hit, GetInstigator()->Controller, this, properties.damageType);
	}

	SpellEnd();
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
		currentSpeed = FMath::Min(properties.maxSpeed, currentSpeed + (properties.acceleration * deltaTime));
		if (homingTarget)
		{
			FVector targetVelocity = (homingTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			targetVelocity += homingTarget->GetVelocity() * targetVelocity.Size() / currentSpeed;

			FVector newVelocity = ProjectileMovementComponent->Velocity + targetVelocity * currentSpeed * deltaTime;
			newVelocity = newVelocity.GetSafeNormal() * currentSpeed;
			currentVelocity = newVelocity;
		}
		else
		{
			FVector newVelocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * currentSpeed;
			currentVelocity = newVelocity;
		}

		OnVelocityChanged();
	}

	
	ProjectileMovementComponent->UpdateComponentVelocity();
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
	
	Destroy();
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

