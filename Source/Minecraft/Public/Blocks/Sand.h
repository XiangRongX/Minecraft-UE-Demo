// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/BlockBase.h"
#include "Sand.generated.h"

UENUM(BlueprintType)
enum class ESandState : uint8
{
	Static,
	Falling
};

/**
 * 
 */
UCLASS()
class MINECRAFT_API ASand : public ABlockBase
{
	GENERATED_BODY()
	
public:
	ASand();
	virtual void Tick(float DeltaTime) override;

	void CheckGravity(AActor* ActorToIgnore = nullptr);

protected:
	virtual void BeginPlay() override;

private:
	ESandState CurrentState = ESandState::Static;
	float FallVelocity = 0.0f;
	const float GravityAccel = -980.0f; // 重力加速度
	const float TerminalVelocity = -1500.0f; // 最大下落速度

	void StartFalling();
	void StopFalling(float TargetZ);
};
