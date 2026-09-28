// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ABCharacterStat.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct ARENABATTLE_API FABCharacterStat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
public:
	FABCharacterStat();

public:
	FABCharacterStat operator+(const FABCharacterStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FABCharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);

		int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; ++i)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}
		return Result;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed = 0.0f;
};
