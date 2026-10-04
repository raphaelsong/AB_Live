// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ABCharacterBase.h"
#include "Interface/ABItemInterface.h"
#include "Item/ABItemData.h"
#include "ABCharacterPlayer.generated.h"

struct FInputActionValue;

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, UABItemData*);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacterPlayer : public AABCharacterBase, public IABItemInterface
{
	GENERATED_BODY()
	
public:
	AABCharacterPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// Item Section
public:
	void TakeItem(class UABItemData* InItemData) override;
	void DrinkPotion(UABItemData* InItemData);
	void ReadScroll(UABItemData* InItemData);
	void EquipWeapon(UABItemData* InItemData);

protected:
	TMap<EItemType, FOnTakeItemDelegate> TakeItemActions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> WeaponComponent;

// PlayerHUD Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UABPlayerHUDWidget> WBP_PlayerHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UABPlayerHUDWidget> ABPlayerHUDWidget;

// Add Component
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> FollowCamera;

#pragma region InputSystem
public:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> IMCDefault;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputAction> AttackAction;
#pragma endregion

};
