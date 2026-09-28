// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ABStatComponent.h"
#include "Singleton/ABGameSingleton.h"

// Sets default values for this component's properties
UABStatComponent::UABStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UABStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupLevel(CurrentLevel);
	SetHp(GetTotalStat().MaxHp);
}

void UABStatComponent::SetupLevel(int32 NewLevel)
{
	CurrentLevel = FMath::Clamp(NewLevel, 1, UABGameSingleton::Get().GetCharacterMaxLevel());

	// 현재 레벨의 BaseStat 설정
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(GetCurrentLevel()));
}

void UABStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, GetTotalStat().MaxHp);

	// Hp 변경 처리
	OnHpChanged.Broadcast(GetCurrentHealth());
}

float UABStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.0f, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		// 죽음처리
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

