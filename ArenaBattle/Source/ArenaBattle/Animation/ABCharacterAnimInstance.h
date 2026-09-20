// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABCharacterAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	TObjectPtr<class ACharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	TObjectPtr<class UCharacterMovementComponent> CharacterMovement;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Data")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Data")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Data")
	float MovingThreshould = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Data")
	float JumpingThreshould = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Data")
	bool bIsIdle = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Data")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Data")
	bool bIsJumping;
};
