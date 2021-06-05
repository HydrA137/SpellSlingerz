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
#include "Spells/SpellBook.h"
#include "SSPlayerState.h"
#include "MPGameMode.h"

//////////////////////////////////////////////////////////////////////////
// ATPCharacter

ATPCharacter::ATPCharacter()
{
	bReplicates = true;

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
	GetCharacterMovement()->JumpZVelocity = 600.f;
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

	spellBook = CreateDefaultSubobject<USpellBook>(TEXT("SpellBook"));
	spellBook->SetIsReplicated(true);
	spellBook->SetNetAddressable();
	this->OwnsComponent(spellBook);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	SpellClass = ASpell::StaticClass();
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	bIsFiringWeapon = false;
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
		AMPGameMode* gameMode = (AMPGameMode*)GetWorld()->GetAuthGameMode();
		float spawnDelay = gameMode->GetSpawnDelay();
		float removeBodyDelay = gameMode->GetRemoveBodyDelay();

		if (lastDamageDealer != this)
		{
			lastDamageDealer->GetPlayerState<ASSPlayerState>()->AddKill();
			this->GetPlayerState<ASSPlayerState>()->AddDeath();
		}
		else
		{
			this->GetPlayerState<ASSPlayerState>()->RemoveKill();
			this->GetPlayerState<ASSPlayerState>()->AddDeath();
		}

		GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ATPCharacter::Respawn, spawnDelay, false);
		GetWorldTimerManager().SetTimer(removeBodyTimerHandle, this, &ATPCharacter::RemoveBody, removeBodyDelay, false);
	}
}

void ATPCharacter::Respawn()
{
	AMPGameMode* gameMode = (AMPGameMode*)GetWorld()->GetAuthGameMode();
	gameMode->RespawnPlayer(this);
}

void ATPCharacter::RemoveBody()
{
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
	if (lastDamageDealer == this)
	{
		return 0;
	}

	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void ATPCharacter::StartFire()
{
	GetWorldTimerManager().SetTimer(firingTimer, this, &ATPCharacter::Firing, 0.01f, true, 0.0f);
}

void ATPCharacter::Firing()
{
	if (!currentSpell)
	{
		ASpell* spell = spellBook->GetSpellByName2("Magic Missile");
		if (spell && spell->IsReady())
		{
			FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 120.0f) + (GetActorUpVector() * 50.0f);
			
			FVector start = FVector(spawnLocation);
			FVector end = start + (GetFollowCamera()->GetForwardVector() * spell->GetProperties().range);
			FVector target = end;

			TArray<AActor*> toIgnore = { this };
			FHitResult result;

			if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), start, end, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, toIgnore, EDrawDebugTrace::None, result, true))
			{
				target = result.ImpactPoint;
			}
			HandleFire(spell, start, end, result);
			spell->Fired();
		}
	}
}

void ATPCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(firingTimer);
	if (currentSpell)
	{
		if (currentSpell->GetProperties().isChargable &&
			currentSpell->IsCharging())
		{
			currentSpell->EndCharge();
			currentSpell = 0;
		}
	}
}

void ATPCharacter::HandleFire_Implementation(ASpell* spellTarget, FVector spawn, FVector target, const FHitResult& hitResult)
{
	FRotator spawnRotation = (target - spawn).Rotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;	
	
	// Get Spell based on type
	currentSpell = GetWorld()->SpawnActor<ASpell>(spellTarget->GetClass(), spawn, spawnRotation, spawnParameters);
	currentSpell->PrepareSpell(target, spellTarget->GetProperties());
	
	if (currentSpell->GetProperties().isHoming && hitResult.Actor.IsValid())
	{
		ATPCharacter* targetPlayer = dynamic_cast<ATPCharacter*>(hitResult.Actor.Get());
		if (targetPlayer)
		{
			currentSpell->SetTarget(hitResult.Actor.Get());
		}
	}

	if (!currentSpell->GetProperties().isChargable)
	{
		currentSpell->Fire();
		currentSpell = 0;
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

