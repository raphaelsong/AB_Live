// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ABCharacterPlayer.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterData/ABCharacterStat.h"
#include "Components/ABStatComponent.h"
#include "UI/ABPlayerHUDWidget.h"
#include <Components/SkeletalMeshComponent.h>
#include "Item/ABItemPotionData.h"
#include "Item/ABItemScrollData.h"
#include "Item/ABItemWeaponData.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 550.0f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FClassFinder<UABPlayerHUDWidget> HUDWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_PlayerHUD.WBP_PlayerHUD_C'"));
	if (HUDWidgetRef.Succeeded())
	{
		WBP_PlayerHUDWidget = HUDWidgetRef.Class;
	}

	// Weapon Component
	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponComponent->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	// Item Section
	TakeItemActions.Add(EItemType::Potion, FOnTakeItemDelegate::CreateUObject(this, &AABCharacterPlayer::DrinkPotion));
	TakeItemActions.Add(EItemType::Scroll, FOnTakeItemDelegate::CreateUObject(this, &AABCharacterPlayer::ReadScroll));
	TakeItemActions.Add(EItemType::Weapon, FOnTakeItemDelegate::CreateUObject(this, &AABCharacterPlayer::EquipWeapon));
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(IMCDefault, 0);
		}
	}

	ABPlayerHUDWidget = CreateWidget<UABPlayerHUDWidget>(PlayerController, WBP_PlayerHUDWidget);
	if (ABPlayerHUDWidget)
	{
		ABPlayerHUDWidget->AddToViewport();
	}

	check(ABPlayerHUDWidget);
	check(StatComponent);

	ABPlayerHUDWidget->UpdateStat(StatComponent->GetBaseStat(), StatComponent->GetModifierStat());
	ABPlayerHUDWidget->UpdateHp(StatComponent->GetCurrentHealth());

	StatComponent->OnHpChanged.AddUObject(ABPlayerHUDWidget, &UABPlayerHUDWidget::UpdateHp);
	StatComponent->OnStatChanged.AddUObject(ABPlayerHUDWidget, &UABPlayerHUDWidget::UpdateStat);
}

void AABCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Look);	
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AABCharacterPlayer::Attack);
}

void AABCharacterPlayer::TakeItem(UABItemData* InItemData)
{
	if (InItemData && TakeItemActions.Contains(InItemData->Type))
	{
		TakeItemActions[InItemData->Type].ExecuteIfBound(InItemData);
	}
}

void AABCharacterPlayer::DrinkPotion(UABItemData* InItemData)
{
	UABItemPotionData* ItemPotionData = Cast<UABItemPotionData>(InItemData);
	if (ItemPotionData)
	{
		if (StatComponent)
		{
			StatComponent->SetHp(StatComponent->GetCurrentHealth() + ItemPotionData->HealAmount);
		}
	}
}

void AABCharacterPlayer::ReadScroll(UABItemData* InItemData)
{
	UABItemScrollData* ItemScrollData = Cast<UABItemScrollData>(InItemData);
	if (ItemScrollData)
	{
		if (StatComponent)
		{
			StatComponent->AddBaseStat(ItemScrollData->BaseStat);
		}
	}
}

void AABCharacterPlayer::EquipWeapon(UABItemData* InItemData)
{
	UABItemWeaponData* ItemWeaponData = Cast<UABItemWeaponData>(InItemData);
	if (ItemWeaponData)
	{
		if (ItemWeaponData->WeaponMesh.IsPending())
		{
			ItemWeaponData->WeaponMesh.LoadSynchronous();
		}

		if (WeaponComponent)
		{
			WeaponComponent->SetSkeletalMesh(ItemWeaponData->WeaponMesh.Get());
		}

		if (StatComponent)
		{
			StatComponent->SetModifierStat(ItemWeaponData->ModifierStat);
		}
	}
}

void AABCharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, MovementVector.X);
	AddMovementInput(RightVector, MovementVector.Y);
}

void AABCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::Attack(const FInputActionValue& Value)
{
	ComboCommand();
}
