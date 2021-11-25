// Fill out your copyright notice in the Description page of Project Settings.


#include "Target.h"
#include "MyGameStateBase.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATarget::ATarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootCapsule"));
	RootComponent = RootCapsule;

	Anchor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Anchor"));
	Anchor->SetupAttachment(RootComponent);

	Target = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target"));
	Target->SetupAttachment(RootComponent);
	Target->SetSimulatePhysics(true);

	PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraintComponent->SetupAttachment(RootCapsule);
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
	bShootable = true;
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATarget::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ATarget::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
                        FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void ATarget::Hitted()
{
	if (bShootable)
	{
		AMyGameStateBase* GS = Cast<AMyGameStateBase>(GetWorld()->GetGameState());
		if (GS)
		{
			GS->Score++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NULL"));
		}
		UE_LOG(LogTemp, Error, TEXT("%d"), GS->Score)

		Target->SetMaterial(0,ShootedMaterial);

		bShootable = false;
		GetWorldTimerManager().SetTimer(RebornTimerHandle,this,&ATarget::Reborn,20.f,false);
	}
}

void ATarget::Reborn()
{
	Target->SetMaterial(0,OriginMaterial);
	bShootable = true;
}



