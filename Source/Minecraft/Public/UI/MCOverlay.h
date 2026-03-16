// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MCOverlay.generated.h"

/**
 * 
 */
UCLASS()
class MINECRAFT_API UMCOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSlotVisuals(int32 NewIndex);
};
