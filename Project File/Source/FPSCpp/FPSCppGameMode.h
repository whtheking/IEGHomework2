// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSCppGameMode.generated.h"

UCLASS(minimalapi)
class AFPSCppGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category=Widget)
	TSubclassOf<UUserWidget> widget;
	
	float LevelTime;
	float Timer;
	

public:
	AFPSCppGameMode();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void GameEnd();

	UFUNCTION(BlueprintNativeEvent)
	void OnVictory();


};



