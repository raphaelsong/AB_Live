// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ABCharacterMonster.h"
#include <Engine/AssetManager.h>

AABCharacterMonster::AABCharacterMonster()
{
	GetMesh()->SetHiddenInGame(true);
}

void AABCharacterMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ensure(MonsterMeshes.Num() > 0);
	int32 RandIndex = FMath::RandRange(0, MonsterMeshes.Num() - 1);
	MonsterMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(MonsterMeshes[RandIndex], FStreamableDelegate::CreateUObject(this, &AABCharacterMonster::MonsterMeshLoadCompleted));
}

void AABCharacterMonster::SetDead()
{
	Super::SetDead();

	// 5초후에 사라지게 하기
	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}

void AABCharacterMonster::MonsterMeshLoadCompleted()
{
	if (MonsterMeshHandle.IsValid())
	{
		USkeletalMesh* MonsterMesh = Cast<USkeletalMesh>(MonsterMeshHandle->GetLoadedAsset());
		if (MonsterMesh)
		{
			GetMesh()->SetSkeletalMesh(MonsterMesh);
			GetMesh()->SetHiddenInGame(false);
		}
	}

	MonsterMeshHandle->ReleaseHandle();
}
