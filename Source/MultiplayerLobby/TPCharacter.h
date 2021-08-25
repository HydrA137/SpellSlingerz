// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Spells/SpellProperties.h"
#include "TPCharacter.generated.h"

UCLASS(config=Game)
class ATPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	FName GetName() { return CharacterName; }

	UFUNCTION(BlueprintCallable)
	void SetName(FName name) { CharacterName = name; }

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ActivateSpell();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	FHitResult GetLookPoint(float distance, float radius);

	UFUNCTION(BlueprintCallable)
	FVector GetSpellCastPoint();

	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetCastingAnim() { return castingAnimMontage; }

	UFUNCTION(BlueprintCallable)
	class ASpell* GetActiveSpell() { return activeSpell; }

	class ASpell* GetPrimarySpell() { return primarySpell; }

	UFUNCTION(BlueprintCallable)
	virtual void OnKill(int score);		
	
protected:

	void OnHealthUpdate();

	/** Getter for Max Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** Getter for Current Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** Setter for Current Health. Clamps the value between 0 and MaxHealth and calls OnHealthUpdate. Should only be called on the server.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float healthValue);

	/** Event for taking damage. Overridden from APawn.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Function for beginning weapon fire.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StartFire();

	void Firing();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void DoCastingAnimation();	

	/** Function for ending weapon fire. Once this is called, the player can use StartFire again.*/
	UFUNCTION(Server, Reliable)
	void Server_StopFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable)
	void HandleFire(ASpell* spellTarget, FVector spawn, FVector _target, const FHitResult& hitResult);
	
	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle FiringTimer;
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void HandleDeath();

	UFUNCTION(Server, Reliable)
	void SendDeath();

	UFUNCTION(NetMulticast, Reliable)
	void SendKill(int score);

	UFUNCTION(NetMulticast, Reliable)
	void KillAllMes();

	UFUNCTION(Server, Reliable)
	void FinaliseDeath();

	void Respawn();

	void RemoveBody();

	// Variables
private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	FTimerHandle firingTimer;

	///////////////////////////
	// Spells
	UPROPERTY(VisibleAnywhere, replicated, BlueprintReadWrite, Category = "Spells", meta = (AllowPrivateAccess = "true"))
	class USpellBook* spellBook;

	class ASpell* primarySpell;
	class ASpell* activeSpell;

protected:
	UPROPERTY(EditAnywhere)
	FName CharacterName;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	class UAnimMontage* castingAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Spells")
	TSubclassOf<class ASpell> SpellClass;

	/** The player's maximum health. This is the highest that their health can be, and the value that their health starts at when spawned.*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

	/** The player's current health. When reduced to 0, they are considered dead.*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	/** RepNotify for changes made to current health.*/
	UFUNCTION()
	void OnRep_CurrentHealth();

	ATPCharacter* lastDamageDealer;

	bool isCasting;

	int CurrentScore;

public:
	ATPCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
};

