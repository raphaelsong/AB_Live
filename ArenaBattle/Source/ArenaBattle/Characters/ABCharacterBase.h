// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABCharacterBase.generated.h"

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

	/* Combo Attack Section*/
public:
	void ComboCommand();

	virtual void ComboBegin();
	virtual void ComboEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	TObjectPtr<class UABComboAttackData> ComboAttackData;

	int32 CurrentCombo = 0;
};
