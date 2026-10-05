// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <Engine/OverlapResult.h>
#include "Characters/ABCharacterMonster.h"
#include "Item/ABItemBox.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
	// Stage Section
	StageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StageMesh"));
	SetRootComponent(StageMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ABAssets/Environment/Stages/SM_SQUARE.SM_SQUARE'"));
	if (StageMeshRef.Succeeded())
	{
		StageMesh->SetStaticMesh(StageMeshRef.Object);
	}

	StageTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTriggerBox->SetupAttachment(GetRootComponent());
	StageTriggerBox->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	StageTriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	StageTriggerBox->SetCollisionProfileName(FName("ABTrigger"));
	StageTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	// Gate Mesh & Trigger Box
	static FName GateSockets[] = { TEXT("+XGate"), TEXT("-XGate"),TEXT("+YGate"), TEXT("-YGate") };

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ABAssets/Environment/Props/SM_GATE.SM_GATE'"));

	for (FName GateSocket : GateSockets)
	{
		UStaticMeshComponent* GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
		GateMesh->SetupAttachment(StageMesh, GateSocket);
		GateMesh->SetStaticMesh(GateMeshRef.Object);
		GateMesh->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f));
		GateMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GateMeshes.Add(GateSocket, GateMesh);

		FName TriggerName = *GateSocket.ToString().Append(TEXT("TriggerBox"));
		UBoxComponent* GateTriggerBox = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTriggerBox->SetupAttachment(StageMesh, GateSocket);
		GateTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		GateTriggerBox->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		GateTriggerBox->SetCollisionProfileName(FName("NoCollision"));
		GateTriggerBox->ComponentTags.Add(GateSocket);
		GateTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
		GateTriggerBoxes.Add(GateTriggerBox);
	}

	// Game State
	CurrentState = EStageState::READY;
	StateChangedActions.Add(EStageState::READY, FOnStateChangedDelegate::CreateUObject(this, &AABStageGimmick::SetStageReady));
	StateChangedActions.Add(EStageState::FIGHT, FOnStateChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight));
	StateChangedActions.Add(EStageState::REWARD, FOnStateChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseReward));
	StateChangedActions.Add(EStageState::NEXT, FOnStateChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext));

	// Fight State
	static ConstructorHelpers::FClassFinder<AABCharacterMonster> MonsterClassRef(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Characters/BP_ABMonster.BP_ABMonster_C'"));
	if (MonsterClassRef.Succeeded())
	{
		MonsterClass = MonsterClassRef.Class;
	}

	// Reward State
	RewardBoxClass = AABItemBox::StaticClass();

	static FName RewardSockets[] = { TEXT("+XReward"), TEXT("-XReward"), TEXT("+YReward"), TEXT("-YReward") };

	for (FName RewardSocket : RewardSockets)
	{
		FVector BoxLocation = StageMesh->GetSocketLocation(RewardSocket);
		RewardBoxLocations.Add(RewardSocket, BoxLocation);
	}
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetState(CurrentState);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetState(EStageState::FIGHT);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	check(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));

	check(StageMesh->DoesSocketExist(SocketName));
	FVector NewLocation = StageMesh->GetSocketLocation(SocketName);

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllObjects,
		FCollisionShape::MakeSphere(775.0f),
		CollisionParam
	);

	if (bResult == false)
	{
		OnStageSpawn(NewLocation);
	}
}

void AABStageGimmick::OpenAllGates()
{
	for (const auto GateMesh : GateMeshes)
	{
		(GateMesh.Value)->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void AABStageGimmick::CloseAllGates()
{
	for (const auto GateMesh : GateMeshes)
	{
		(GateMesh.Value)->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AABStageGimmick::SetState(EStageState InNewState)
{
	CurrentState = InNewState;
	if (StateChangedActions.Contains(InNewState))
	{
		StateChangedActions[InNewState].ExecuteIfBound();
	}
}

void AABStageGimmick::SetStageReady()
{
	StageTriggerBox->SetCollisionProfileName(FName("ABTrigger"));
	for (auto GateTriggerBox : GateTriggerBoxes)
	{
		GateTriggerBox->SetCollisionProfileName(FName("NoCollision"));
	}

	OpenAllGates();
}

void AABStageGimmick::SetFight()
{
	StageTriggerBox->SetCollisionProfileName(FName("NoCollision"));
	for (auto GateTriggerBox : GateTriggerBoxes)
	{
		GateTriggerBox->SetCollisionProfileName(FName("NoCollision"));
	}

	CloseAllGates();

	GetWorld()->GetTimerManager().SetTimer(MonsterSpawnTimerHandle, this, &AABStageGimmick::OnMonsterSpawn, MonsterSpawnTime, false);
}

void AABStageGimmick::SetChooseReward()
{
	StageTriggerBox->SetCollisionProfileName(FName("NoCollision"));
	for (auto GateTriggerBox : GateTriggerBoxes)
	{
		GateTriggerBox->SetCollisionProfileName(FName("NoCollision"));
	}

	CloseAllGates();

	SpawnRewardBoxes();
}

void AABStageGimmick::SetChooseNext()
{
	StageTriggerBox->SetCollisionProfileName(FName("NoCollision"));
	for (auto GateTriggerBox : GateTriggerBoxes)
	{
		GateTriggerBox->SetCollisionProfileName(FName("ABTrigger"));
	}

	OpenAllGates();
}

void AABStageGimmick::OnStageSpawn(FVector NewLocation)
{
	FTransform NewTransform(NewLocation);
	AABStageGimmick* NewGimmick = GetWorld()->SpawnActorDeferred<AABStageGimmick>(AABStageGimmick::StaticClass(), NewTransform);

	if (NewGimmick)
	{
		NewGimmick->SetStageNum(GetStageNum() + 1);
		NewGimmick->FinishSpawning(NewTransform);
	}
}

void AABStageGimmick::OnMonsterSpawn()
{
	const FTransform SpawnTransform(GetActorLocation() + FVector::UpVector * 88.0f);
	AABCharacterMonster* NewMonster = GetWorld()->SpawnActorDeferred<AABCharacterMonster>(MonsterClass, SpawnTransform);

	if (NewMonster)
	{
		NewMonster->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnMonsterDestroyed);
		NewMonster->SetLevel(CurrentStageNum);
		NewMonster->FinishSpawning(SpawnTransform);
	}
}

void AABStageGimmick::OnMonsterDestroyed(AActor* DestroyedActor)
{
	SetState(EStageState::REWARD);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		FTransform SpawnTransform(GetActorLocation() + RewardBoxLocation.Value + FVector(0.0f, 0.0f, 30.0f));
		AABItemBox* RewardBoxActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, SpawnTransform);
		if (RewardBoxActor)
		{
			RewardBoxActor->Tags.Add(RewardBoxLocation.Key);
			RewardBoxActor->GetTriggerBox()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			
			RewardBoxes.Add(RewardBoxActor);
		}
	}

	for (const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			RewardBox.Get()->FinishSpawning(RewardBox.Get()->GetActorTransform());
		}
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			AABItemBox* ValidItemBox = RewardBox.Get();
			AActor* OverlappedBox = OverlappedComponent->GetOwner();
			if (OverlappedBox != ValidItemBox)
			{
				ValidItemBox->Destroy();
			}
		}
	}

	SetState(EStageState::NEXT);
}


