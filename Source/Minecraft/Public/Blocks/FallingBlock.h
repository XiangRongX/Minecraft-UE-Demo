// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/BlockBase.h"
#include "FallingBlock.generated.h"

UENUM(BlueprintType)
enum class EFallingBlockState : uint8
{
	Static,
	Falling
};

/**
 * 
 */
UCLASS()
class MINECRAFT_API AFallingBlock : public ABlockBase
{
	GENERATED_BODY()
	
public:
	AFallingBlock();
	virtual void Tick(float DeltaTime) override;

	void CheckGravity(AActor* ActorToIgnore = nullptr);

protected:
	virtual void BeginPlay() override;

private:
	EFallingBlockState CurrentState = EFallingBlockState::Static;
	float FallVelocity = 0.0f;
	const float GravityAccel = -980.0f; // 重力加速度
	const float TerminalVelocity = -1500.0f; // 最大下落速度

	void StartFalling();
	void StopFalling(float TargetZ);
};
