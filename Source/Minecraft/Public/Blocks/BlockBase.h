// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Block.h"
#include "BlockBase.generated.h"

class UNiagaraSystem;

UENUM(BlueprintType)
enum class EToolType : uint8
{
	Pickaxe,
	Axe,
	Shovel,
	Hoe
};

UENUM(BlueprintType)
enum class EToolMaterial : uint8
{
	Wood,
	Gold,
	Stone,
	Copper,
	Iron,
	Diamond,
	Netherite
};

UCLASS()
class MINECRAFT_API ABlockBase : public AActor, public IBlock
{
	GENERATED_BODY()
	
public:	
	ABlockBase();
	virtual void Destroyed() override;

	void ShowHighLight(bool bEnable);
	void UpdateBreakVisuals(float Progress);
	float CalculateDigTime(bool bHasTool, EToolType Tool = EToolType::Shovel, EToolMaterial ToolMaterial = EToolMaterial::Wood) const;

	// 放置与破坏音效相同
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MC|Sounds")
	TObjectPtr<USoundBase> BlockBreakSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MC|Sounds")
	TObjectPtr<USoundBase> BlockDigSound;

	UPROPERTY(EditDefaultsOnly, Category = "MC|Sounds")
	float DigSoundInterval = 0.2f;

protected:
	virtual void BeginPlay() override;

	void NotifyBlocksToFall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MC|Components")
	TObjectPtr<UStaticMeshComponent> BlockMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MC|Effects")
	TObjectPtr<UNiagaraSystem> BlockBreakEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MC|Properties")
	float Hardness = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MC|Properties")
	bool bRequiresCorrectTool = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MC|Properties")
	EToolType RequiredTool = EToolType::Shovel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MC|Properties")
	EToolMaterial RequiredToolMaterial = EToolMaterial::Wood;

private:
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
};
