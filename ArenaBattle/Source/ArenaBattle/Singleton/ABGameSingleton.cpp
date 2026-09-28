// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/ABGameSingleton.h"
#include "ABGameSingleton.h"

UABGameSingleton::UABGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/CharacterData/DT_ABCharacterStat.DT_ABCharacterStat'"));

	if (DataTableRef.Succeeded())
	{
		const UDataTable* DT = DataTableRef.Object;
		check(DT->GetRowMap().Num() > 0);

		TArray<uint8*> ValueArray;
		DT->GetRowMap().GenerateValueArray(ValueArray);
		Algo::Transform(ValueArray, CharacterStatTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FABCharacterStat*>(Value);
			}
		);
	}

	CharacterMaxLevel = CharacterStatTable.Num();
	ensure(CharacterMaxLevel > 0);
}
