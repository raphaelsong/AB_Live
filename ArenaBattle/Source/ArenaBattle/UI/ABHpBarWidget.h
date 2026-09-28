// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ABHpBarWidget.generated.h"

struct FABCharacterStat;

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHpBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);

public:
	void UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
	void UpdateHp(float NewCurrentHp);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> HpBar;

private:
	float CurrentHp = 0.0f;
	float MaxHp = 0.0f;
};
