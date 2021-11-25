// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"

UCLASS()
class FPSCPP_API AGunBase : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Mesh)
	USkeletalMesh* GunMesh;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Location)
	USceneComponent* MuzzleLocation;
	
	// Sets default values for this actor's properties
	AGunBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
