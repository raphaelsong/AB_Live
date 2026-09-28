// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterData/ABCharacterStat.h"
#include "ABGameSingleton.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UABGameSingleton();

	FORCEINLINE static UABGameSingleton& Get()
	{
		UABGameSingleton* GameSingleton = CastChecked<UABGameSingleton>(GEngine->GameSingleton);
		if (GameSingleton)
		{
			return *GameSingleton;
		}

		return *NewObject<UABGameSingleton>();
	}

public:
	FORCEINLINE int32 GetCharacterMaxLevel() { return CharacterMaxLevel; }

	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const
	{
		if (CharacterStatTable.IsValidIndex(InLevel - 1))
			return CharacterStatTable[InLevel - 1];
		else
			return FABCharacterStat();
	}

private:
	TArray<FABCharacterStat> CharacterStatTable;
	int32 CharacterMaxLevel = 1;
};
