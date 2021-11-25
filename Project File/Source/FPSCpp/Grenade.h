// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleEmitter.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Grenade.generated.h"

UCLASS()
class FPSCPP_API AGrenade : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	                       FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
	                       const FHitResult& Hit) override;
	
	void Explore();

	UStaticMeshComponent* GetStaticMeshComponent();
	USphereComponent* GetSphereComponent();

public:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite)
	USphereComponent* DamageRange;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category=Asset)
	UParticleSystem* ParticleEmitter;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category=Asset)
	USoundBase* ExplodeSound;
	FTimerHandle ExplodeTimerHandle;
};
