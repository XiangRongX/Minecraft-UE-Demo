// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCCharacter.generated.h"

class ABlockBase;
class UCameraComponent;

UCLASS()
class MINECRAFT_API AMCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMCCharacter();
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

protected:


private:
    UPROPERTY(VisibleAnywhere, Category = "MC|Camera")
    TObjectPtr<class UCameraComponent> FirstPersonCamera;

    void UpdateBlockHighlight();
	TObjectPtr<ABlockBase> LastFrameHighlightedBlock;
    TObjectPtr<ABlockBase> ThisFrameHighlightedBlock;
};
