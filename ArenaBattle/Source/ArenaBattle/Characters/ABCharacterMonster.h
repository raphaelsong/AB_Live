// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ABCharacterBase.h"
#include <Engine/StreamableManager.h>
#include "ABCharacterMonster.generated.h"

/**
 * 
 */
UCLASS(Config = Monster)
class ARENABATTLE_API AABCharacterMonster : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterMonster();

protected:
	virtual void PostInitializeComponents() override;

public:
	virtual void SetDead() override;

public:
	void MonsterMeshLoadCompleted();

private:
	float DeadEventDelayTime = 5.0f;

protected:
	UPROPERTY(Config)
	TArray<FSoftObjectPath> MonsterMeshes;

	TSharedPtr<FStreamableHandle> MonsterMeshHandle;
};
