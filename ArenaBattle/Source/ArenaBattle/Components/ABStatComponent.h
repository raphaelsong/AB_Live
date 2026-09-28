// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterData/ABCharacterStat.h"
#include "ABStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FABCharacterStat& /*BaseStat*/, const FABCharacterStat& /*ModifierStat*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
public:
	FORCEINLINE float GetCurrentLevel() { return CurrentLevel; }
	FORCEINLINE float GetCurrentHealth() { return CurrentHp; }
	FORCEINLINE const FABCharacterStat& GetBaseStat() const { return BaseStat; }
	FORCEINLINE const FABCharacterStat& GetModifierStat() const { return ModifierStat; }
	FORCEINLINE const FABCharacterStat GetTotalStat() const { return GetBaseStat() + GetModifierStat(); }

public:
	FORCEINLINE void AddBaseStat(const FABCharacterStat InAddBaseStat)
	{
		BaseStat = BaseStat + InAddBaseStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}

	FORCEINLINE void AddModifierStat(const FABCharacterStat InAddModifierStat)
	{
		ModifierStat = ModifierStat + InAddModifierStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}

	FORCEINLINE void SetBaseStat(const FABCharacterStat& InBaseStat)
	{
		BaseStat = InBaseStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}

	FORCEINLINE void SetModifierStat(const FABCharacterStat& InModifierStat)
	{
		ModifierStat = InModifierStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}

	FORCEINLINE void AddHp(float InHealAmount)
	{
		CurrentHp = FMath::Clamp<float>(CurrentHp + InHealAmount, 0.0f, GetTotalStat().MaxHp);
		OnHpChanged.Broadcast(CurrentHp);
	}

public:
	void SetupLevel(int32 NewLevel);
	void SetHp(float NewHp);

public:
	float ApplyDamage(float InDamage);

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;
	FOnStatChangedDelegate OnStatChanged;

protected:
	UPROPERTY(Transient, VisibleAnywhere, Category = Stat)
	float CurrentLevel = 1;

	UPROPERTY(Transient, VisibleAnywhere, Category = Stat)
	float CurrentHp = 0;

	UPROPERTY(Transient, VisibleAnywhere, Category = Stat)
	FABCharacterStat BaseStat;

	UPROPERTY(Transient, VisibleAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;
};
