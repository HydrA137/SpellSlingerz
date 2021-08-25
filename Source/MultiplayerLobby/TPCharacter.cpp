// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPCharacter.h"
#include "GameFramework//Actor.h"
#include "HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "TestProjectile.h"
#include "Spells/Spell.h"
#include "Spells/MagicMissile.h"
#include "Spells/SpellProperties.h"
#include "Spells/GunGameSpellBook.h"
#include "SSPlayerState.h"
#include "SSGameModeBase.h"

//////////////////////////////////////////////////////////////////////////
// ATPCharacter

ATPCharacter::ATPCharacter()
{
	bReplicates = true;
	isCasting = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	CharacterName = FName(*FString::FromInt(rand() % 100));
	lastDamageDealer = 0;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 1800.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	spellBook = CreateDefaultSubobject<UGunGameSpellBook>(TEXT("SpellBook"));
	spellBook->SetIsReplicated(true);
	spellBook->SetNetAddressable();
	this->OwnsComponent(spellBook);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	SpellClass = ASpell::StaticClass();
	MaxHealth = 1.0f;
	CurrentHealth = MaxHealth;

	CurrentScore = 0;
}

////////////////////////////////////////////////////////////////////////
// Networking

void ATPCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ATPCharacter, CurrentHealth);
}

void ATPCharacter::OnHealthUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);

			HandleDeath();
			SendDeath();
		}
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//Functions that occur on all machines. 
	/*
		Any special functionality that should occur as a result of damage or death should be placed here.
	*/
}

void ATPCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ATPCharacter::HandleDeath()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Handing Death");

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	if (IsLocallyControlled())
	{
		GetCameraBoom()->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true), "Pelvis");
	}
}

void ATPCharacter::SendDeath_Implementation()
{
	KillAllMes();
}

void ATPCharacter::SendKill_Implementation(int score)
{
	OnKill(score);
}

void ATPCharacter::KillAllMes_Implementation()
{
	if (!IsLocallyControlled())
	{
		HandleDeath();
	}
	else
	{
		FinaliseDeath();
	}
}

void ATPCharacter::FinaliseDeath_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FTimerHandle respawnTimerHandle;
		FTimerHandle removeBodyTimerHandle;
		ASSGameModeBase* gameMode = (ASSGameModeBase*)GetWorld()->GetAuthGameMode();
		float spawnDelay = gameMode->GetSpawnDelay();
		float removeBodyDelay = gameMode->GetRemoveBodyDelay();

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Finalising Death");

		if (lastDamageDealer != this)
		{
			lastDamageDealer->SendKill(1);
			//lastDamageDealer->GetPlayerState<ASSPlayerState>()->AddKill();			
			//this->GetPlayerState<ASSPlayerState>()->AddDeath();
		}
		else
		{
			this->SendKill(-1);
			//this->GetPlayerState<ASSPlayerState>()->RemoveKill();			
			//this->GetPlayerState<ASSPlayerState>()->AddDeath();
		}

		GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ATPCharacter::Respawn, spawnDelay, false);
		GetWorldTimerManager().SetTimer(removeBodyTimerHandle, this, &ATPCharacter::RemoveBody, removeBodyDelay, false);
	}
}

void ATPCharacter::OnKill(int score)
{
	if (IsLocallyControlled())
	{
		if (GetPlayerState()->Score + score >= 0.0f) 
		{
			GetPlayerState()->Score += score;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Current Score: %f"), GetPlayerState()->Score));
		}

		spellBook->OnKill(score);
		primarySpell = 0;
	}
}

void ATPCharacter::Respawn()
{
	ASSGameModeBase* gameMode = (ASSGameModeBase*)GetWorld()->GetAuthGameMode();
	gameMode->RespawnPlayer(this, GetClass());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Respawning");
}

void ATPCharacter::RemoveBody()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Remove Body");
	this->Destroy();
}

//////////////////////////////////////////////////////////////////////////
// Stats
void ATPCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ATPCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	lastDamageDealer = Cast<ATPCharacter>(Cast<AActor>(DamageCauser)->GetOwner());

	FString ownerMessage = "Last: " + lastDamageDealer->GetName().ToString() + ", This: " + this->GetName().ToString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, ownerMessage);
	/*if (lastDamageDealer == this)
	{
		return 0;
	}*/

	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void ATPCharacter::StartFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Start Fire");
	GetWorldTimerManager().SetTimer(firingTimer, this, &ATPCharacter::Firing, 0.01f, true, 0.0f);
}

void ATPCharacter::Firing()
{
	if (!primarySpell)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Why though?");
		primarySpell = spellBook->GetPrimarySpell();
		primarySpell->GetProperties().cooldownTimer = 0.0f;
	}
	
	if (primarySpell->IsReady() && !isCasting)
	{
		// Start Firing Animation
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Start Cast Spell");
		DoCastingAnimation();
	}
}

void ATPCharacter::DoCastingAnimation()
{
	bool bPlayedSuccessfully = false;
	isCasting = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && castingAnimMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Starting Animation");
		const float MontageLength = AnimInstance->Montage_Play(castingAnimMontage, FMath::Min(primarySpell->GetProperties().fireRate * 1.2f, 2.0f), EMontagePlayReturnType::Duration, 0.0f);
		bPlayedSuccessfully = (MontageLength > 0.f);
	}
}

void ATPCharacter::ActivateSpell()
{
	if (this)
	{
		isCasting = false;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Activate Spell");
		FVector spawnLocation = GetSpellCastPoint();

		FVector start = GetFollowCamera()->GetComponentLocation();
		FVector end = start + (GetFollowCamera()->GetForwardVector() * primarySpell->GetProperties().range);
		FVector target = end;

		TArray<AActor*> toIgnore = { this };
		FHitResult result;

		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), start, end, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, toIgnore, EDrawDebugTrace::None, result, true))
		{
			target = result.ImpactPoint;
		}
		HandleFire(primarySpell, spawnLocation, target, result);
		primarySpell->Fired();
		
	}
}

void ATPCharacter::Server_StopFire_Implementation()
{
	StopFire();
}

void ATPCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(firingTimer);

	if (primarySpell)
	{
		if (primarySpell->GetProperties().isChargable &&
			primarySpell->IsCharging())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Primary Charge Ending");
			primarySpell->Fired();
		}
		else if (primarySpell->GetProperties().isHeld)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Primary Held Ending");
			primarySpell->GetProperties().Reset();
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Resume(castingAnimMontage);
		}
	}

	if (HasAuthority()) {
		if (activeSpell)
		{
			if (activeSpell->GetProperties().isChargable &&
				activeSpell->IsCharging())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Active Charge Ending");
				activeSpell->EndCharge();
				activeSpell = 0;
			}
			else if (activeSpell->GetProperties().isHeld)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Active Held Ending");
				activeSpell->SpellEnd();
				activeSpell = 0;
			}
		}
	}
	else
	{
		Server_StopFire();
	}	
}

void ATPCharacter::HandleFire_Implementation(ASpell* spellTarget, FVector spawn, FVector target, const FHitResult& hitResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "firing");
	FRotator spawnRotation = (target - spawn).Rotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;
	
	// Get Spell based on type
	activeSpell = GetWorld()->SpawnActor<ASpell>(spellTarget->GetClass(), spawn, spawnRotation, spawnParameters);
	activeSpell->PrepareSpell(target, spellTarget->GetProperties());
	
	if (activeSpell->GetProperties().isHoming && hitResult.Actor.IsValid())
	{
		ATPCharacter* targetPlayer = dynamic_cast<ATPCharacter*>(hitResult.Actor.Get());
		if (targetPlayer)
		{
			dynamic_cast<AProjectileSpell*>(activeSpell)->SetHomingTarget(hitResult.Actor.Get());
		}
	}

	if (activeSpell->GetProperties().isChargable == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Shoulndt see this");
		activeSpell->Fire();
		
		if (!activeSpell->GetProperties().isHeld)
		{
			// if not held we fire and forget.
			activeSpell = 0;
		}
	}
	else if (activeSpell->GetProperties().isHeld == true)
	{
		activeSpell->BeginCharge();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Handle firing projectiles
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPCharacter::StopFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPCharacter::LookUpAtRate);
}

void ATPCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATPCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATPCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

FHitResult ATPCharacter::GetLookPoint(float distance, float radius)
{
	TArray<AActor*> toIgnore = { this };
	FHitResult result;
	FVector start = GetFollowCamera()->GetComponentLocation();
	FVector end = start + (GetFollowCamera()->GetForwardVector() * distance);
	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), start, end, radius, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, toIgnore, EDrawDebugTrace::None, result, true))
	{
		return result;
	}

	return result; 
}

FVector ATPCharacter::GetSpellCastPoint()
{
	return GetMesh()->GetBoneLocation("Character1_RightHand");
}
