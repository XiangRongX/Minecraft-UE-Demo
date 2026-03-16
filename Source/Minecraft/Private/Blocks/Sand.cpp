// Fill out your copyright notice in the Description page of Project Settings.


#include "Blocks/Sand.h"

ASand::ASand()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void ASand::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == ESandState::Falling)
    {
        // 1. 计算下落位移
        FallVelocity = FMath::FInterpTo(FallVelocity, TerminalVelocity, DeltaTime, 0.5f);
        float DeltaZ = FallVelocity * DeltaTime;
        FVector CurrentLoc = GetActorLocation();
        FVector NextLoc = CurrentLoc + FVector(0, 0, DeltaZ);

        // 2. 预测碰撞：向下发射一条射线，距离等于这一帧要走的距离 + 50（方块半径）
        FHitResult Hit;
        FVector TraceEnd = CurrentLoc + FVector(0, 0, DeltaZ - 50.1f);
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        if (GetWorld()->LineTraceSingleByChannel(Hit, CurrentLoc, TraceEnd, ECC_Visibility, Params))
        {
            // 撞到地面了！
            // 计算精准的对齐高度：碰撞点的 Z + 50
            StopFalling(Hit.ImpactPoint.Z + 50.0f);
        }
        else
        {
            // 空中飞行：保持 X, Y 不变，只更新 Z
            SetActorLocation(NextLoc);
        }
    }
}

void ASand::BeginPlay()
{
	Super::BeginPlay();

	CheckGravity();
}


void ASand::CheckGravity(AActor* ActorToIgnore)
{
    if (CurrentState == ESandState::Falling) return;

    FHitResult Hit;
    FVector Start = GetActorLocation();
    FVector End = Start - FVector(0, 0, 55.0f); // 探测脚下 5 厘米

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    if (ActorToIgnore)
    {
        Params.AddIgnoredActor(ActorToIgnore);
    }

    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        StartFalling();
    }
}

void ASand::StartFalling()
{
    CurrentState = ESandState::Falling;
    FallVelocity = 0.0f;
    SetActorTickEnabled(true);

    BlockMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

    NotifyBlocksToFall();
}

void ASand::StopFalling(float TargetZ)
{
    CurrentState = ESandState::Static;
    FallVelocity = 0.0f;
    SetActorTickEnabled(false);

    // 执行精准对齐
    FVector FinalLoc = GetActorLocation();
    FinalLoc.Z = TargetZ; // 先设置 Z 为触碰面高度

    // 使用 GridSnap 强制对齐到 100n + 50
    FinalLoc.X = FMath::GridSnap(FinalLoc.X - 50.0f, 100.0f) + 50.0f;
    FinalLoc.Y = FMath::GridSnap(FinalLoc.Y - 50.0f, 100.0f) + 50.0f;
    FinalLoc.Z = FMath::GridSnap(FinalLoc.Z - 50.0f, 100.0f) + 50.0f;

    SetActorLocation(FinalLoc);
    BlockMesh->SetCollisionResponseToAllChannels(ECR_Block);
}
