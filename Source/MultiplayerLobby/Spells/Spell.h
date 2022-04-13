// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellProperties.h"
#include "Explosion.h"
#include "Spell.generated.h"

UCLASS()
class MULTIPLAYERLOBBY_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpell();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category = "Spell_Events")
	virtual void PrepareSpell(FVector target, const FSpellProperties& spellProps);

	UFUNCTION(Category = "Spell_Events")
	virtual void Fire();

	UFUNCTION(Category = "Spell_Events")
	virtual void FireAt(FVector target);

	UFUNCTION(Category = "Spell_Events")
	virtual void OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Category = "Spell_Events")
	virtual void BeginCharge();

	virtual void Charging(float detalTime);

	UFUNCTION(Category = "Spell_Events")
	virtual void EndCharge();

	UFUNCTION(BlueprintCallable)
	virtual void SpellEnd();

	UFUNCTION()
	void DestroySpell();
	
	UFUNCTION()
	bool IsCharging() { return isCharging; }

	UFUNCTION()
	bool IsReady() { return properties.IsReady(); }

	UFUNCTION()
	void Fired() { properties.Fired(); }

	UFUNCTION()
	FSpellProperties& GetProperties() { return properties; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable)
	void HandleExplosion(const FExplosionProperties& explosionProps);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Variable
protected:
	////////////////////////////////////////////////
	// Visuals
	

	// Static Mesh used to provide a visual representation of the object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* staticMesh;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	TSubclassOf<AExplosion> explosionClass;

	////////////////////////////////////////////////
	// Movement/Targeting
	FVector target;

	////////////////////////////////////////////////
	// Properties
	// Spell Properties component 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Properties")
	FSpellProperties properties;
	
	/////////////////////////////////////////////////
	// States
	bool isCharging;
};
