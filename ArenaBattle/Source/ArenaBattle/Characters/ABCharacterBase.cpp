// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterData/ABComboAttackData.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Capsule Component
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// Movement Component
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);

	// Mesh Component
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/ABAssets/Warriors/Character/CompleteCharacters/SK_CharM_Warrior.SK_CharM_Warrior'"));

	if (MeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshRef.Object);
	}

	// Animation
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Animation/ABP_ABCharacter.ABP_ABCharacter_C'"));

	if (AnimClassRef.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboAttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/AM_ComboAttack.AM_ComboAttack'"));
	
	if (ComboAttackMontageRef.Succeeded())
	{
		ComboAttackMontage = ComboAttackMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UABComboAttackData> ComboAttackDataRef(TEXT("/Script/ArenaBattle.ABComboAttackData'/Game/CharacterData/DA_ComboAttack.DA_ComboAttack'"));

	if (ComboAttackDataRef.Succeeded())
	{
		ComboAttackData = ComboAttackDataRef.Object;
	}
}

void AABCharacterBase::ComboCommand()
{
	if (CurrentCombo == 0)
	{
		ComboBegin();
		return;
	}

	// 콤보 연결 처리

}

void AABCharacterBase::ComboBegin()
{
	CurrentCombo = 1;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	const float AttackSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(ComboAttackMontage, AttackSpeedRate);
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboAttackMontage);
}

void AABCharacterBase::ComboEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	CurrentCombo = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

