// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MCPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AMCCharacter;
class ABlockBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHotbarIndexChanged, int32, NewIndex);

UENUM(BlueprintType)
enum class EMCGameMode : uint8
{
    Survival,
    Creative
};

/**
 * 
 */
UCLASS()
class MINECRAFT_API AMCPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintAssignable, Category = "MC|Events")
	FOnHotbarIndexChanged OnHotbarIndexChanged;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputMappingContext> BaseMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputMappingContext> SurvivalMappingContext; 

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputMappingContext> CreativeMappingContext; 

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> PlaceAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> CreativeBreakAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> SurvivalBreakAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> ChangeBlockAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> ChangeGameModeAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> MouseScrollAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Input")
    TObjectPtr<UInputAction> NumberAction;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Jump();
	void StopJumping();
    void OnRightClick();
	void OnLeftClick();          // 创造模式
    void OnLeftClickStarted();   // 生存模式：开始挖掘
    void OnLeftClickTriggered(); // 生存模式：持续挖掘中
    void OnLeftClickCompleted(); // 生存模式：停止挖掘
    void OnMiddleClick();
	void OnChangeGameMode();
    void OnHotBarScroll(const FInputActionValue& Value);
    void OnHotBarNumber(const FInputActionValue& Value);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MC|Construction")
    TSubclassOf<ABlockBase> BlockToPlace;

private:
    TObjectPtr<AMCCharacter> Character;
    EMCGameMode CurrentMode = EMCGameMode::Creative;
    void UpdateMappingContexts();

    TObjectPtr<ABlockBase> CurrentMiningBlock;
    float MiningTimeSpent = 0.0f;
	float MiningTimeRequired = 0.0f;
	float MiningSpeed = 1.0f; // 可以根据工具类型调整挖掘速度
    float LastDigSoundTime = 0.0f;
    ABlockBase* GetTargetBlockUnderCursor();

    int32 CurrentHotbarIndex = 0; // 当前选中的索引 (0-8)
    const int32 HotbarSize = 9;   // 快捷栏总长度
};
