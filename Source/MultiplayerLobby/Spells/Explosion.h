#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionProperties.h"
#include "Explosion.generated.h"

UCLASS()
class MULTIPLAYERLOBBY_API AExplosion : public AActor
{
	GENERATED_BODY()
	
	// functions
public:	
	// Sets default values for this actor's properties
	AExplosion();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Explosion_Events")
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetProperties(const FExplosionProperties& props) { properties = props; }

	UFUNCTION(BlueprintCallable)
	const FExplosionProperties& GetProperties() { return properties; }

	UFUNCTION(BlueprintCallable)
	void Explode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGrowSpeedChanged();

	UFUNCTION()
	void OnSizeChanged();
	
	// Variables
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* SphereComponent;

	// Static Mesh used to provide a visual representation of the object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USceneComponent* explosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystemComponent* explodeEffectComponent;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystem* explosionEffect;

	UPROPERTY(ReplicatedUsing = OnGrowSpeedChanged)
	float currentGrowSpeed;

	UPROPERTY(ReplicatedUsing = OnSizeChanged)
	float currentSize;

	////////////////////////////////////////////////
	// Properties
	// Explosion Properties component 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	FExplosionProperties properties;

	FVector meshStartScale;
	bool exploding = false;
};
