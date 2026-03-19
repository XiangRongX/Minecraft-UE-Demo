// Fill out your copyright notice in the Description page of Project Settings.


#include "Blocks/BlockBase.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInterface.h"
#include "Blocks/FallingBlock.h"

ABlockBase::ABlockBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	RootComponent = BlockMesh;
	BlockMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BlockMesh->SetCustomDepthStencilValue(252);
	BlockMesh->MarkRenderStateDirty();
}

void ABlockBase::Destroyed()
{
	FVector BlockLocation = GetActorLocation();
	if (BlockBreakEffect)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BlockBreakEffect, BlockLocation);
		if (NiagaraComp && BlockMesh)
		{
			UMaterialInterface* Mat = BlockMesh->GetMaterial(0);
			if (Mat)
			{
				NiagaraComp->SetVariableMaterial(FName("Material"), Mat);
			}
		}
	}
	if (BlockBreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BlockBreakSound, BlockLocation);
	}

	Super::Destroyed();

	// 通知上方的沙子下落
	NotifyBlocksToFall();
}

void ABlockBase::ShowHighLight(bool bEnable)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("ShowHighlight"), bEnable ? 1.f : 0.f);
	}
}

void ABlockBase::UpdateBreakVisuals(float Progress)
{
	if (DynamicMaterial)
	{
		if(Progress<=0.f)
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("ShowCrack"), 0.f);
		}
		else
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("ShowCrack"), 1.f);
			float Stage = FMath::FloorToFloat(Progress * 10.0f);
			DynamicMaterial->SetScalarParameterValue(TEXT("CrackStage"), Stage);
		}
	}
}

float ABlockBase::CalculateDigTime(bool bHasTool, EToolType Tool, EToolMaterial ToolMaterial) const
{
	float s = 1.f; // 基础挖掘速度
	if (bHasTool)
	{
		switch (ToolMaterial)
		{
		case EToolMaterial::Wood:
			s = 2.f;
			break;
		case EToolMaterial::Stone:
			s = 4.f;
			break;
		case EToolMaterial::Copper:
			s = 5.f;
			break;
		case EToolMaterial::Iron:
			s = 6.f;
			break;
		case EToolMaterial::Diamond:
			s = 8.f;
			break;
		case EToolMaterial::Netherite:
			s = 9.f;
			break;
		case EToolMaterial::Gold:
			s = 12.f;
			break;
		}
	}

	float r = 30.f; // 基础除数，如果有挖掘惩罚为100
	if (bRequiresCorrectTool)
	{
		if (!bHasTool) r = 100.f;
		else if (Tool != RequiredTool) r = 100.f;
	}

	float t = FMath::CeilToInt(Hardness * r / s); // 所需tick
	return t / 20; // 转换为秒
}

void ABlockBase::BeginPlay()
{
	Super::BeginPlay();

	if (BlockMesh)
	{
		DynamicMaterial = BlockMesh->CreateDynamicMaterialInstance(0);
	}
}

void ABlockBase::NotifyBlocksToFall()
{
	FHitResult Hit;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart + FVector(0, 0, 100.f);

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility))
	{
		// 如果上方是沙子，触发它的检查
		AFallingBlock* FallingBlock = Cast<AFallingBlock>(Hit.GetActor());
		if (FallingBlock)
		{
			FallingBlock->CheckGravity(this);
		}
	}
}



