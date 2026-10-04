// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "ABItemPotionData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABItemPotionData : public UABItemData
{
	GENERATED_BODY()
	
public:
	UABItemPotionData();

public:
	UPROPERTY(EditAnywhere, Category = HP)
	float HealAmount;
};
