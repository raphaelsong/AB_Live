// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ABCharacterMonster.h"

AABCharacterMonster::AABCharacterMonster()
{
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
