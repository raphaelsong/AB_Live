// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ABCharacterBase.h"
#include "ABCharacterMonster.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacterMonster : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterMonster();

public:
	virtual void SetDead() override;

private:
	float DeadEventDelayTime = 5.0f;
};
