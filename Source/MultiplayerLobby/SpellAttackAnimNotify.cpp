// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellAttackAnimNotify.h"
#include "Spells/Spell.h"
#include "TPCharacter.h"

void USpellAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    ATPCharacter* character = dynamic_cast<ATPCharacter*>(MeshComp->GetOwner());
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Checking if Character Exsists");

    if (character)
    {
        character->ActivateSpell();
        ASpell* spell = character->GetPrimarySpell();

        if (spell && spell->GetProperties().isHeld)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Pausing Animation");
            UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance();
            AnimInstance->Montage_Pause(character->GetCastingAnim());
        }
    }

}
