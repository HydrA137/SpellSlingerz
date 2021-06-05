
#include "Explosion.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "../TPCharacter.h"

// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(1.0f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetVisibility(true);
	SphereComponent->SetHiddenInGame(false);
	RootComponent = SphereComponent;

	explosionComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ExplosionComponent"));
	explosionComponent->SetupAttachment(RootComponent);
	meshStartScale = explosionComponent->GetRelativeScale3D();

	//Definition for the Mesh that will serve as our visual representation.
	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Set the Static Mesh and its position/scale if we successfully found a mesh asset to use.
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -1.0f));
		StaticMesh->SetRelativeScale3D(FVector(0.02f, 0.02f, 0.02f));
	}*/

	//Registering the Projectile Impact function on a Hit event.
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AExplosion::OnOverlapBegin);

		currentGrowSpeed = properties.initialGrowSpeed;
		currentSize = 1.0f;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		explosionEffect = DefaultExplosionEffect.Object;
	}
}

void AExplosion::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	//DOREPLIFETIME(ASpell, currentSpeed);
	DOREPLIFETIME(AExplosion, currentGrowSpeed);
	DOREPLIFETIME(AExplosion, currentSize);
}

void AExplosion::Explode()
{
	exploding = true;
	currentGrowSpeed = properties.initialGrowSpeed;
	currentSize = 1.0f;
}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();
	
	explodeEffectComponent = UGameplayStatics::SpawnEmitterAttached(explosionEffect, RootComponent, FName(TEXT("P_Explosion")), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true, EPSCPoolMethod::AutoRelease);
}

void AExplosion::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ATPCharacter::StaticClass()))
	{
		float damage = properties.maxDamage;
		float distance = FVector::Distance(OtherActor->GetActorLocation(), this->GetActorLocation());
		float falloffStartDistance = properties.maxSize * properties.damageFalloffStart;
		if (distance <= falloffStartDistance)
		{
			damage = properties.maxDamage;
		}
		else
		{
			damage = FMath::Max(properties.minDamage, properties.maxDamage * ((distance - falloffStartDistance) / (properties.maxSize - falloffStartDistance)));
		}
		UGameplayStatics::ApplyDamage(OtherActor, damage, GetInstigator()->Controller, this, properties.damageType);
	}
}

void AExplosion::OnGrowSpeedChanged()
{
}

void AExplosion::OnSizeChanged()
{
	SphereComponent->SetSphereRadius(currentSize);
	explosionComponent->SetRelativeScale3D(FVector(meshStartScale.X * currentSize, meshStartScale.Y * currentSize, meshStartScale.Z * currentSize));
}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority && exploding)
	{
		currentGrowSpeed = FMath::Min(properties.maxGrowSpeed, currentGrowSpeed + (properties.acceleration * DeltaTime));

		currentSize = FMath::Min(properties.maxSize, currentSize + currentGrowSpeed * DeltaTime);

		SphereComponent->SetSphereRadius(currentSize);
		explosionComponent->SetRelativeScale3D(FVector(meshStartScale.X * currentSize, meshStartScale.Y * currentSize, meshStartScale.Z * currentSize));
		
		if (currentSize == properties.maxSize)
		{
			Destroy();
		}
	}
}

