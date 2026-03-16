// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MCPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/MCCharacter.h"
#include "Interfaces/BlockHighlight.h"
#include "Blocks/BlockBase.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

void AMCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AMCCharacter>(GetCharacter());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(BaseMappingContext, 0);
		Subsystem->AddMappingContext(CreativeMappingContext, 1);
	}
}

void AMCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMCPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMCPlayerController::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMCPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMCPlayerController::StopJumping);

		EnhancedInputComponent->BindAction(PlaceAction, ETriggerEvent::Started, this, &AMCPlayerController::OnRightClick);

		EnhancedInputComponent->BindAction(CreativeBreakAction, ETriggerEvent::Started, this, &AMCPlayerController::OnLeftClick);

		EnhancedInputComponent->BindAction(SurvivalBreakAction, ETriggerEvent::Started, this, &AMCPlayerController::OnLeftClickStarted);
		EnhancedInputComponent->BindAction(SurvivalBreakAction, ETriggerEvent::Triggered, this, &AMCPlayerController::OnLeftClickTriggered);
		EnhancedInputComponent->BindAction(SurvivalBreakAction, ETriggerEvent::Completed, this, &AMCPlayerController::OnLeftClickCompleted);
		EnhancedInputComponent->BindAction(SurvivalBreakAction, ETriggerEvent::Canceled, this, &AMCPlayerController::OnLeftClickCompleted);

		EnhancedInputComponent->BindAction(ChangeBlockAction, ETriggerEvent::Started, this, &AMCPlayerController::OnMiddleClick);
		EnhancedInputComponent->BindAction(ChangeGameModeAction, ETriggerEvent::Started, this, &AMCPlayerController::OnChangeGameMode);
		EnhancedInputComponent->BindAction(MouseScrollAction, ETriggerEvent::Triggered, this, &AMCPlayerController::OnHotBarScroll);
		EnhancedInputComponent->BindAction(NumberAction, ETriggerEvent::Triggered, this, &AMCPlayerController::OnHotBarNumber);

	}
}

void AMCPlayerController::Move(const FInputActionValue& Value)
{
	if (!Character) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	Character->AddMovementInput(Character->GetActorForwardVector(), MovementVector.X);
	Character->AddMovementInput(Character->GetActorRightVector(), MovementVector.Y);
}

void AMCPlayerController::Look(const FInputActionValue& Value)
{
	if (!Character) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AMCPlayerController::Jump()
{
	if (Character)
	{
		Character->Jump();
	}
}

void AMCPlayerController::StopJumping()
{
	if (Character)
	{
		Character->StopJumping();
	}
}

void AMCPlayerController::OnRightClick()
{
	if (!Character->GetFirstPersonCamera() || !BlockToPlace) return;

	FHitResult Hit;
	FVector Start = Character->GetFirstPersonCamera()->GetComponentLocation();
	FVector End = Start + (Character->GetFirstPersonCamera()->GetForwardVector() * 500.f);

	// 执行射线检测
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		if (!Hit.GetActor()->Implements<UBlockHighlight>()) return;
		// 1. 获取被点击方块的中心位置
		FVector TargetBlockLoc = Hit.GetActor()->GetActorLocation();

		// 2. 根据法线（点击的面）计算理论上的新中心
		// 例如：点击顶面 [0,0,1]，新中心就是原中心 + [0,0,100]
		FVector RawSpawnLocation = TargetBlockLoc + (Hit.ImpactNormal * 100.f);

		// 3. 核心：网格化对齐 (Grid Snapping)
		// 逻辑：(坐标 - 50) / 100 -> 四舍五入 -> * 100 + 50
		auto SnapToMCGrid = [](float Val) {
			return FMath::RoundToFloat((Val - 50.f) / 100.f) * 100.f + 50.f;
			};

		FVector FinalSpawnLocation;
		FinalSpawnLocation.X = SnapToMCGrid(RawSpawnLocation.X);
		FinalSpawnLocation.Y = SnapToMCGrid(RawSpawnLocation.Y);
		FinalSpawnLocation.Z = SnapToMCGrid(RawSpawnLocation.Z);

		// 4. 生成方块
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABlockBase* NewBlock = GetWorld()->SpawnActor<ABlockBase>(BlockToPlace, FinalSpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (NewBlock->BlockBreakSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), NewBlock->BlockBreakSound, NewBlock->GetActorLocation());
		}
	}
}

void AMCPlayerController::OnLeftClick()
{
	if (!Character->GetFirstPersonCamera()) return;

	FHitResult Hit;
	FVector Start = Character->GetFirstPersonCamera()->GetComponentLocation();
	FVector End = Start + (Character->GetFirstPersonCamera()->GetForwardVector() * 500.f);

	// 执行射线检测
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		if (!Hit.GetActor()->Implements<UBlockHighlight>()) return;
		AActor* BlockToBreak = Hit.GetActor();
		BlockToBreak->Destroy();
	}
}

void AMCPlayerController::OnLeftClickStarted()
{
	CurrentMiningBlock = GetTargetBlockUnderCursor();
	MiningTimeSpent = 0.0f;

	if (CurrentMiningBlock)
	{
		// 基岩检测
		/*if (CurrentMiningBlock->Hardness < 0)
		{
			CurrentMiningBlock = nullptr;
			return;
		}*/
		MiningTimeRequired = CurrentMiningBlock->CalculateDigTime(false);
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, TEXT("开始挖掘..."));
	}
}

void AMCPlayerController::OnLeftClickTriggered()
{
	if (!CurrentMiningBlock) return;

	// 每帧检查准星是否还在该方块上
	ABlockBase* HitBlock = GetTargetBlockUnderCursor();
	if (HitBlock != CurrentMiningBlock)
	{
		OnLeftClickCompleted(); // 准星移开了，重置
		return;
	}

	// 累加时间
	MiningTimeSpent += GetWorld()->GetDeltaSeconds();

	// 计算进度 (0.0 - 1.0)
	float Progress = FMath::Clamp(MiningTimeSpent / MiningTimeRequired, 0.0f, 1.0f);

	// 更新方块表现（裂纹）
	CurrentMiningBlock->UpdateBreakVisuals(Progress);

	// 播放音效
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastDigSoundTime >= CurrentMiningBlock->DigSoundInterval)
	{
		if (CurrentMiningBlock->BlockDigSound)
		{
			// 在方块位置播放挖掘声
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				CurrentMiningBlock->BlockDigSound,
				CurrentMiningBlock->GetActorLocation(),
				0.5f, // 音量稍微小一点
				FMath::RandRange(0.4f, 0.8f) // 随机音高让声音不单调
			);
		}
		LastDigSoundTime = CurrentTime;
	}

	// 判定是否挖完
	if (Progress >= 1.0f)
	{
		CurrentMiningBlock->Destroy(); 
		OnLeftClickCompleted();        
	}
}

void AMCPlayerController::OnLeftClickCompleted()
{
	if (CurrentMiningBlock)
	{
		// 重置方块裂纹表现
		CurrentMiningBlock->UpdateBreakVisuals(0.0f);
	}

	CurrentMiningBlock = nullptr;
	MiningTimeSpent = 0.0f;
	MiningTimeRequired = 0.0f;
	GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Orange, TEXT("挖掘停止"));
}

void AMCPlayerController::OnMiddleClick()
{
	if (!Character->GetFirstPersonCamera()) return;

	FHitResult Hit;
	FVector Start = Character->GetFirstPersonCamera()->GetComponentLocation();
	FVector End = Start + (Character->GetFirstPersonCamera()->GetForwardVector() * 500.f);

	// 执行射线检测
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		AActor* BlockToChange = Hit.GetActor();
		if (!BlockToChange->Implements<UBlockHighlight>()) return;
		if (BlockToPlace = Cast<ABlockBase>(BlockToChange)->GetClass())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Selected Block: %s"), *BlockToPlace->GetName()));
		}
	}
}

void AMCPlayerController::OnChangeGameMode()
{
	if (CurrentMode == EMCGameMode::Creative)
	{
		CurrentMode = EMCGameMode::Survival;
	}
	else
	{
		CurrentMode = EMCGameMode::Creative;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Switched to %s Mode"), CurrentMode == EMCGameMode::Creative ? TEXT("Creative") : TEXT("Survival")));
	UpdateMappingContexts();
}

void AMCPlayerController::OnHotBarScroll(const FInputActionValue& Value)
{
	float ScrollValue = Value.Get<float>();
	if (FMath::IsNearlyZero(ScrollValue)) return;

	// 根据滚动方向更新索引
	// 向上滚动 (ScrollValue > 0) -> 索引减少 (向左切)
	// 向下滚动 (ScrollValue < 0) -> 索引增加 (向右切)
	int32 Direction = (ScrollValue > 0.0f) ? -1 : 1;

	// 实现 0-8 的循环逻辑
	CurrentHotbarIndex = (CurrentHotbarIndex + Direction + HotbarSize) % HotbarSize;

	OnHotbarIndexChanged.Broadcast(CurrentHotbarIndex); 
}

void AMCPlayerController::OnHotBarNumber(const FInputActionValue& Value)
{
	float RawValue = Value.Get<float>();

	// 2. 取整得到索引 (0-8)
	int32 NewIndex = FMath::RoundToInt(RawValue);

	// 3. 更新并广播
	if (NewIndex != CurrentHotbarIndex)
	{
		CurrentHotbarIndex = NewIndex;

		if (OnHotbarIndexChanged.IsBound())
		{
			OnHotbarIndexChanged.Broadcast(CurrentHotbarIndex);
		}
	}
}

void AMCPlayerController::UpdateMappingContexts()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// 先清理旧的功能层，保留基础层
		Subsystem->RemoveMappingContext(SurvivalMappingContext);
		Subsystem->RemoveMappingContext(CreativeMappingContext);

		// 基础层永远在优先级 0
		if (!Subsystem->HasMappingContext(BaseMappingContext))
			Subsystem->AddMappingContext(BaseMappingContext, 0);

		// 功能层在优先级 1，会覆盖基础层中的同名按键（如果有）
		if (CurrentMode == EMCGameMode::Survival)
		{
			Subsystem->AddMappingContext(SurvivalMappingContext, 1);
		}
		else
		{
			Subsystem->AddMappingContext(CreativeMappingContext, 1);
		}
	}
}

ABlockBase* AMCPlayerController::GetTargetBlockUnderCursor()
{
	if (!Character || !Character->GetFirstPersonCamera()) return nullptr;

	FHitResult Hit;
	FVector Start = Character->GetFirstPersonCamera()->GetComponentLocation();
	FVector End = Start + (Character->GetFirstPersonCamera()->GetForwardVector() * 500.f);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		if (Hit.GetActor()->Implements<UBlockHighlight>())
		{
			return Cast<ABlockBase>(Hit.GetActor());
		}
	}
	return nullptr;
}
