// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "CharacterData/ABCharacterStat.h"
#include "ABItemWeaponData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABItemWeaponData : public UABItemData
{
	GENERATED_BODY()
	
public:
	UABItemWeaponData();

public:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TObjectPtr<class USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;
};
