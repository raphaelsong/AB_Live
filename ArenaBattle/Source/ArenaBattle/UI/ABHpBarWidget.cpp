// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"
#include "CharacterData/ABCharacterStat.h"
#include "Components/ProgressBar.h"
#include "ABPlayerHUDWidget.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UABHpBarWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	MaxHp = (BaseStat + ModifierStat).MaxHp;

	if (HpBar)
	{
		HpBar->SetPercent(CurrentHp / MaxHp);
	}
}

void UABHpBarWidget::UpdateHp(float NewCurrentHp)
{
	CurrentHp = NewCurrentHp;

	if (HpBar)
	{
		HpBar->SetPercent(CurrentHp / MaxHp);
	}
}
