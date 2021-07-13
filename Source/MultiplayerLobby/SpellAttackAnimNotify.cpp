// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellAttackAnimNotify.h"
#include "Spells/Spell.h"
#include "TPCharacter.h"

void USpellAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    ATPCharacter* character = dynamic_cast<ATPCharacter*>(MeshComp->GetOwner());

    character->ActivateSpell();
    ASpell* spell = character->GetActiveSpell();

    if (spell && spell->GetProperties().isHeld)
    {
        UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance();
        AnimInstance->Montage_Pause(character->GetCastingAnim());
    }

}
