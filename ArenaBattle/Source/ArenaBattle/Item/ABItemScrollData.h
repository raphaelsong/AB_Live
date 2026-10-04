// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "CharacterData/ABCharacterStat.h"
#include "ABItemScrollData.generated.h"


UCLASS()
class ARENABATTLE_API UABItemScrollData : public UABItemData
{
	GENERATED_BODY()
	
public:
	UABItemScrollData();

public:
	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat BaseStat;
};
