// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAttackInterface.h"
#include "CharacterData/ABCharacterStat.h"
#include "Components/ABStatComponent.h"
#include "ABCharacterBase.generated.h"

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

public:
	virtual void PostInitializeComponents() override;

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/* Character Stat Section*/
public:
	FORCEINLINE TObjectPtr<UABStatComponent> GetStatComponent()
	{
		return StatComponent;
	}

	FORCEINLINE int32 GetLevel()
	{
		if (StatComponent)
			return StatComponent->GetCurrentLevel();
		return 0;
	}

	FORCEINLINE void SetLevel(int32 InNewLevel)
	{
		if (StatComponent)
			StatComponent->SetupLevel(InNewLevel);
	}

public:
	void ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UABStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> HpBarWidgetComponent;

	/* Dead Section*/
public:
	virtual void SetDead();
	void PlayDeadAnimation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dead)
	TObjectPtr<class UAnimMontage> DeadMontage;

	/* Combo Attack Section*/
public:
	void ComboCommand();

	virtual void ComboBegin();
	virtual void ComboEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	virtual void SetComboCheckTimer();
	virtual void ComboCheck();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	TObjectPtr<class UABComboAttackData> ComboAttackData;

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;

// 인터페이스 함수 선언부
public:
	// Inherited via IABAttackInterface
	void AttackHitCheck() override;
};
