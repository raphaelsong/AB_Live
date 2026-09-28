// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ABCharacterBase.h"
#include <Engine/DamageEvents.h>
#include <Kismet/GameplayStatics.h>
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterData/ABComboAttackData.h"
#include <Components/WidgetComponent.h>
#include "UI/ABHpBarWidget.h"

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
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCapsule"));

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

	// Dead Section
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Succeeded())
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Character Stat Section
	StatComponent = CreateDefaultSubobject<UABStatComponent>(TEXT("StatComponent"));

	// Widget Component
	HpBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpBarComponent"));
	HpBarWidgetComponent->SetupAttachment(GetMesh());
	HpBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	static ConstructorHelpers::FClassFinder<UABHpBarWidget> HpBarWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_HpBar.WBP_HpBar_C'"));
	if (HpBarWidgetRef.Succeeded())
	{
		HpBarWidgetComponent->SetWidgetClass(HpBarWidgetRef.Class);
		HpBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		HpBarWidgetComponent->SetDrawAtDesiredSize(true);
		HpBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(StatComponent);
	StatComponent->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	StatComponent->OnStatChanged.AddUObject(this, &AABCharacterBase::ApplyStat);

	check(HpBarWidgetComponent);
	HpBarWidgetComponent->InitWidget();
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(HpBarWidgetComponent->GetUserWidgetObject());
	if (HpBarWidget)
	{
		StatComponent->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHp);
		StatComponent->OnStatChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateStat);
	}
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	StatComponent->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void AABCharacterBase::ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	float MovementSpeed = (BaseStat + ModifierStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SetActorEnableCollision(false);
	PlayDeadAnimation();

	if (HpBarWidgetComponent)
	{
		HpBarWidgetComponent->SetHiddenInGame(true);
	}
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->Montage_Play(DeadMontage);
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
	if (ComboTimerHandle.IsValid() == false)
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}

}

void AABCharacterBase::ComboBegin()
{
	CurrentCombo = 1;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	const float AttackSpeedRate = StatComponent->GetTotalStat().AttackSpeed;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(ComboAttackMontage, AttackSpeedRate);
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboAttackMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void AABCharacterBase::ComboEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	CurrentCombo = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboAttackData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = StatComponent->GetTotalStat().AttackSpeed;
	float ComboEffectiveTime = (ComboAttackData->EffectiveFrameCount[ComboIndex] / ComboAttackData->FrameRate) / AttackSpeedRate;

	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();

	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboAttackData->MaxComboCount);
		
		FName NextSectionName = *FString::Printf(TEXT("%s%d"), *ComboAttackData->MontageSectionNamePrefix, CurrentCombo);

		AnimInstance->Montage_JumpToSection(NextSectionName, ComboAttackMontage);

		HasNextComboCommand = false;
		SetComboCheckTimer();
	}
}

void AABCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	FCollisionQueryParams CollParam;
	CollParam.AddIgnoredActor(this);

	const float AttackRange = StatComponent->GetTotalStat().AttackRange;
	const float AttackRadius = StatComponent->GetTotalStat().AttackRadius;
	const float AttackDamage = StatComponent->GetTotalStat().Attack;

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool bHitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(AttackRadius),
		CollParam);

	if (bHitDetected)
	{
		FDamageEvent DamageEvent;
		UGameplayStatics::ApplyDamage(OutHitResult.GetActor(), AttackDamage, GetController(), this, nullptr);
	}

#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;

	FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
#endif //  ENABLE_DRAW_DEBUG

}

