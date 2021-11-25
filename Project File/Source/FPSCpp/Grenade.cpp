// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"

#include "HealthComponent.h"
#include "Target.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGrenade::AGrenade()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereColl"));
	RootComponent = SphereComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetRelativeLocation(FVector(0.f));

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->SetRelativeLocation(FVector(0.f));

	DamageRange = CreateDefaultSubobject<USphereComponent>(TEXT("DamageRange"));
	DamageRange->SetupAttachment(RootComponent);
	DamageRange->SetRelativeLocation(FVector(0.f));
	DamageRange->SetSphereRadius(1000.f);
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &AGrenade::Explore, 5.f, 0);
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrenade::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
                         FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void AGrenade::Explore()
{
	RadialForceComponent->FireImpulse();
	TArray<AActor*> TargetArray;
	TSubclassOf<ATarget> TargetClass;
	DamageRange->GetOverlappingActors(TargetArray,TargetClass);
	
	for (auto& Target : TargetArray)
	{
		if (Target->IsA<ATarget>())
		{
			Cast<ATarget>(Target)->Hitted();
		}
	}
	//带有healthcomp的pawn伤害判定
	TArray<AActor*> PawnArray;
	TSubclassOf<APawn> PawnClass;
	DamageRange->GetOverlappingActors(PawnArray,PawnClass);
	UE_LOG(LogTemp,Error,TEXT("num:%d"),PawnArray.Num())
	
	for (auto& Pawn : PawnArray)
	{
		
		TSubclassOf<class UHealthComponent> HealthClass=UHealthComponent::StaticClass();
		UActorComponent* GettedComponent;
		GettedComponent=Pawn->GetComponentByClass(HealthClass);
		
		if(GettedComponent)
		{
			UHealthComponent* HealthComponent=Cast<UHealthComponent>(GettedComponent);

			float Distence=(Pawn->GetActorLocation()-GetActorLocation()).Size();
			float Damagevalue=150*(DamageRange->GetScaledSphereRadius()-Distence)/DamageRange->GetScaledSphereRadius();
			
			HealthComponent->ChangeHealth(Damagevalue);
		}
		
	}
	
	if(ParticleEmitter)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ParticleEmitter,GetActorLocation());
	}
	if(ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),ExplodeSound,GetActorLocation());
	}
	
	Destroy();
}

UStaticMeshComponent* AGrenade::GetStaticMeshComponent()
{
	return StaticMesh;
}

USphereComponent* AGrenade::GetSphereComponent()
{
	return SphereComponent;
}
