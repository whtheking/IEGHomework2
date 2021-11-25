// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthSystem.h"

// Sets default values
AHealthSystem::AHealthSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FullHealth=100;
	FullShield=100;
	CurrentHealth=100;
	CurrentShield=100;
	bShieldActive=true;
}

// Called when the game starts or when spawned
void AHealthSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHealthSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

