// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthSystem.generated.h"

UCLASS()
class FPSCPP_API AHealthSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthSystem();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Heaalth)
	float FullHealth;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Heaalth)
	float CurrentHealth;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Heaalth)
	float FullShield;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Heaalth)
	float CurrentShield;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Heaalth)
	bool bShieldActive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
