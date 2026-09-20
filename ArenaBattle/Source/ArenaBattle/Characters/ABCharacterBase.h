// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAttackInterface.h"
#include "ABCharacterBase.generated.h"

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

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
