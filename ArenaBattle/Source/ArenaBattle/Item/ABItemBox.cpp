// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemBox.h"
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Particles/ParticleSystemComponent.h>

// Sets default values
AABItemBox::AABItemBox()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));
	TriggerBox->SetCollisionProfileName(FName("ABTrigger"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(FName("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ABAssets/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
	if (MeshRef.Succeeded())
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ABAssets/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
	if (EffectRef.Succeeded())
	{
		Effect->SetTemplate(EffectRef.Object);
		Effect->bAutoActivate = false;
	}
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnBoxBeginOverlap);
	}
}

void AABItemBox::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Mesh)
	{
		Mesh->SetHiddenInGame(true);
	}

	if (Effect)
	{
		Effect->Activate(true);
		Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
	}

	SetActorEnableCollision(false);
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}


