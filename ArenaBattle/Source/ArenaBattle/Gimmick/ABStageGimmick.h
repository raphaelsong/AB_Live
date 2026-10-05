// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

UENUM(BlueprintType)
enum class EStageState : uint8
{
	READY = 0,
	FIGHT,
	REWARD,
	NEXT,
};

DECLARE_DELEGATE(FOnStateChangedDelegate);

UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABStageGimmick();

public:
	virtual void OnConstruction(const FTransform& Transform) override;

	/**Stage Section*/
public:
	UFUNCTION()
	void OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OpenAllGates();
	void CloseAllGates();

protected:
	UPROPERTY(VisibleAnywhere, Category = Stage)
	TObjectPtr<class UStaticMeshComponent> StageMesh;

	UPROPERTY(VisibleAnywhere, Category = Stage)
	TObjectPtr<class UBoxComponent> StageTriggerBox;

	UPROPERTY(VisibleAnywhere, Category = Stage)
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> GateMeshes;

	UPROPERTY(VisibleAnywhere, Category = Stage)
	TArray<TObjectPtr<class UBoxComponent>> GateTriggerBoxes;

	/**Game State Section*/
public:
	void SetState(EStageState InNewState);
	void SetStageReady();
	void SetFight();
	void SetChooseReward();
	void SetChooseNext();

protected:
	UPROPERTY(EditAnywhere, Category = Stage)
	EStageState CurrentState;

	TMap<EStageState, FOnStateChangedDelegate> StateChangedActions;

	// Next State
public:
	void OnStageSpawn(FVector NewLocation);

	// Fight State
public:
	UFUNCTION()
	void OnMonsterSpawn();

	UFUNCTION()
	void OnMonsterDestroyed(AActor* DestroyedActor);

protected:
	UPROPERTY(EditAnywhere, Category = Fight)
	TSubclassOf<class AABCharacterMonster> MonsterClass;

	UPROPERTY(EditAnywhere, Category = Fight)
	float MonsterSpawnTime = 2.0f;

	FTimerHandle MonsterSpawnTimerHandle;

	// Reward State
public:
	void SpawnRewardBoxes();

	UFUNCTION()
	void OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, Category = Reward)
	TSubclassOf<class AABItemBox> RewardBoxClass;

	UPROPERTY(VisibleAnywhere, Category = Reward)
	TArray<TWeakObjectPtr<class AABItemBox>> RewardBoxes;

	TMap<FName, FVector> RewardBoxLocations;

	/** StageStat Section*/
public:
	FORCEINLINE int32 GetStageNum() const { return CurrentStageNum; }
	FORCEINLINE void SetStageNum(int32 NewStageNum) { CurrentStageNum = NewStageNum; }

protected:
	UPROPERTY(VisibleInstanceOnly, Category = Stage)
	int32 CurrentStageNum = 0;
};
