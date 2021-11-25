// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Target.generated.h"

UCLASS()
class FPSCPP_API ATarget : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATarget();

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UCapsuleComponent* RootCapsule;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UStaticMeshComponent* Anchor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* Target;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UPhysicsConstraintComponent* PhysicsConstraintComponent;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Materials")
	UMaterialInterface* OriginMaterial;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Materials")
	UMaterialInterface* ShootedMaterial;
	
	bool bShootable;
	FTimerHandle RebornTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	                       FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
	                       const FHitResult& Hit) override;

	void Hitted();

	void Reborn();
};
