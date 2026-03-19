// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MCCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blocks/BlockBase.h"

AMCCharacter::AMCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;
}

void AMCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateBlockHighlight();
}

void AMCCharacter::UpdateBlockHighlight()
{
	if (!FirstPersonCamera) return;

	FHitResult Hit;
	FVector Start = FirstPersonCamera->GetComponentLocation();
	FVector End = Start + (FirstPersonCamera->GetForwardVector() * 500.f);

	ThisFrameHighlightedBlock = nullptr;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);
	if (!bHit || !Hit.GetActor()->Implements<UBlock>())
	{
		// 如果没有击中任何物体，则直接返回
		if (LastFrameHighlightedBlock)
		{
			LastFrameHighlightedBlock->ShowHighLight(false);
		}
		LastFrameHighlightedBlock = nullptr;
		return;
	}

	if (AActor* HitActor = Hit.GetActor())
	{
		if (ABlockBase* HitBlock = Cast<ABlockBase>(HitActor))
		{
			ThisFrameHighlightedBlock = HitBlock;
		}
	}

	// 如果与上一帧不同则更新（避免重复调用）
	if (ThisFrameHighlightedBlock != LastFrameHighlightedBlock)
	{
		if (LastFrameHighlightedBlock)
		{
			LastFrameHighlightedBlock->ShowHighLight(false);
		}
		if (ThisFrameHighlightedBlock)
		{
			ThisFrameHighlightedBlock->ShowHighLight(true);
		}

		LastFrameHighlightedBlock = ThisFrameHighlightedBlock;
	}
}

