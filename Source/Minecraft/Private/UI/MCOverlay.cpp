// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MCOverlay.h"
#include "Character/MCPlayerController.h"

void UMCOverlay::NativeConstruct()
{
    Super::NativeConstruct();

    if (AMCPlayerController* PC = Cast<AMCPlayerController>(GetOwningPlayer()))
    {
        PC->OnHotbarIndexChanged.AddDynamic(this, &UMCOverlay::UpdateSlotVisuals);
    }
}
