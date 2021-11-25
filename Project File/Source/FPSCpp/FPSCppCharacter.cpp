// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSCppCharacter.h"
#include "FPSCppProjectile.h"
#include "Target.h"
#include "Grenade.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Blueprint/UserWidget.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPSCppCharacter

AFPSCppCharacter::AFPSCppCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	Mesh1P = this->GetMesh();
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	Mesh1P->SetRelativeLocation(FVector(-0.f, -0.f, -90.f));

	Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	Gun->SetupAttachment(Mesh1P, TEXT("Gun"));

	TPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(Mesh1P);
	TPSCameraComponent->SetupAttachment(CameraSpringArm);
	TPSCameraComponent->AttachToComponent(CameraSpringArm, FAttachmentTransformRules::KeepRelativeTransform);
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->bEnableCameraLag = true;
	CameraSpringArm->TargetArmLength = 300.f;
	CameraSpringArm->SetRelativeLocation(FVector(60.f, 0.f, 160.f));

	ZoomCameraLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ZoomCameraLocation"));
	ZoomCameraLocation->SetupAttachment(Mesh1P, TEXT("head"));

	ZoomInCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ZoomInCamera"));
	ZoomInCamera->SetupAttachment(ZoomCameraLocation);
	ZoomInCamera->bUsePawnControlRotation = true;
	ZoomInCamera->SetRelativeLocation(FVector(13.f, 5.f, -5.f));
	ZoomInCamera->SetRelativeRotation(FRotator(-30.f, 75.f, -110.f));

	MainCamera = TPSCameraComponent;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(Gun);
	MuzzleLocation->SetRelativeLocation(FVector(0.f, 50.f, 0.f));

	GrenadeLocation = CreateDefaultSubobject<USceneComponent>(TEXT("GrenadeLocation"));
	GrenadeLocation->SetupAttachment(Gun);
	GrenadeLocation->SetRelativeLocation(FVector(10.f, 30.f, 10.f));
	
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	CurrentAmmo = 30;
	PerAmmo = 30;
	FullAmmo = 120;
	GrenadeCount = 5;
	ReloadTime = 2;
	ReloadTimer = 0;
	
	bAbleToFire = true;
	bAbleToZoomIn = true;
	bAbleToJump = true;
	bAbleToCrouch = true;
	bAbleToRun=true;
	bAbleToUseGrenade=true;
	HitImpulse = 100000.0f;
	ShootingDistance=10000.0f;
}

void AFPSCppCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if (PlayerStateWidget)
	{
		CreateWidget<UUserWidget>(GetWorld(), PlayerStateWidget)->AddToViewport();
	}
}


void AFPSCppCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCppCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSCppCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCppCharacter::Reload);

	PlayerInputComponent->BindAction("Grenade", IE_Pressed, this, &AFPSCppCharacter::Grenade);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AFPSCppCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AFPSCppCharacter::StopWalk);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSCppCharacter::OnCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSCppCharacter::StopCrouch);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &AFPSCppCharacter::OnZoom);
	PlayerInputComponent->BindAction("ZoomIn", IE_Released, this, &AFPSCppCharacter::StopZoom);

	// PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AFPSCppCharacter::Run);
	// PlayerInputComponent->BindAction("Run", IE_Released, this, &AFPSCppCharacter::StopRun);


	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCppCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCppCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSCppCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSCppCharacter::LookUpAtRate);
}

void AFPSCppCharacter::OnFire()
{
	if (!bAbleToFire)
		return;
	if (GetCharacterMovement()->Velocity.Size() <= 300)
	{
		if (Ironsights_FireMontage)
		{
			PlayAnimMontage(Ironsights_FireMontage);
		}
	}
	else if (GetCharacterMovement()->Velocity.Size() > 300)
	{
		if (Hip_FireMontage)
		{
			PlayAnimMontage(Hip_FireMontage);
		}
	}

	// if (ProjectileClass != nullptr)
	// {
	// 	UWorld* const World = GetWorld();
	// 	if (World != nullptr)
	// 	{
	// 		const FRotator SpawnRotation = GetControlRotation();
	// 		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	// 		const FVector SpawnLocation = ((MuzzleLocation != nullptr)
	// 			                               ? MuzzleLocation->GetComponentLocation()
	// 			                               : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
	// 		//Set Spawn Collision Handling Override
	// 		FActorSpawnParameters ActorSpawnParams;
	// 		ActorSpawnParams.SpawnCollisionHandlingOverride =
	// 			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	//
	// 		// spawn the projectile at the muzzle
	// 		AFPSCppProjectile* Projectile = World->SpawnActor<AFPSCppProjectile>(
	// 			ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	// 		if (Projectile)
	// 		{
	// 			FVector2D ViewportSize;
	// 			FVector ScreenToWorldLoc;
	// 			FVector ScreenToWorldDir;
	// 			GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	// 			GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(
	// 				ViewportSize.X / 2, ViewportSize.Y / 2, ScreenToWorldLoc, ScreenToWorldDir);
	// 			Projectile->FireInDirection(ScreenToWorldDir);
	// 		}
	// 		UE_LOG(LogTemp, Error, TEXT("shoot"))
	// 		bIsFiring = true;
	// 	}
	// }

// 移动时弹道偏移
	float AimOffSet = FireOffset();
	FVector HitLocationOffset = 0.02 * FVector(FMath::RandRange(-AimOffSet, AimOffSet),
	                                           FMath::RandRange(-AimOffSet, AimOffSet),
	                                           FMath::RandRange(-AimOffSet, AimOffSet));
	FHitResult HitResult;
	FVector Start = MainCamera->GetComponentLocation();
	FVector End = MainCamera->GetComponentLocation() + (MainCamera->GetForwardVector() + HitLocationOffset).
		GetSafeNormal() * ShootingDistance;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	if (HitResult.GetComponent())
	{
		AActor* HittedActor=HitResult.GetActor();
		UPrimitiveComponent* HittedComponent=HitResult.GetComponent();
		
		if (HittedComponent->IsSimulatingPhysics())
		{
			float PointImpulse;
			PointImpulse = HitImpulse * (ShootingDistance - (HitResult.ImpactPoint - GetActorLocation()).Size()) / ShootingDistance;
			UE_LOG(LogTemp, Error, TEXT("%f"), PointImpulse);
			HittedComponent->AddImpulseAtLocation((End - Start).GetSafeNormal() * PointImpulse,
			                                               GetActorLocation());
		}

		if (HittedActor->IsA<ATarget>())
		{
			Cast<ATarget>(HittedActor)->Hitted();
		}

		//存在生命组件
		TSubclassOf<class UHealthComponent> HealthClass=UHealthComponent::StaticClass();
		UActorComponent* GettedComponent;
		GettedComponent=HittedActor->GetComponentByClass(HealthClass);
		
		if(GettedComponent)
		{
			UHealthComponent* HealthComponent=Cast<UHealthComponent>(GettedComponent);
			if(HitResult.BoneName=="head")
			{
				HealthComponent->ChangeHealth(50.f);
			}
			else
			{
				HealthComponent->ChangeHealth(10.f);
			}
			
		}

		

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HittedParticle, HitResult.ImpactPoint,
		                                         FRotator::ZeroRotator, FVector(.2f));
	}


	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (ShootParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(ShootParticle, MuzzleLocation, "Mozzle", FVector(0.f),
		                                       FRotator::ZeroRotator, FVector(.1f));
	}

	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShake);

	CurrentAmmo -= 1;

	if (CurrentAmmo == 0)
	{
		Reload();
	}
}

void AFPSCppCharacter::StopFire()
{
	bIsFiring = false;
}


void AFPSCppCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSCppCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCppCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCppCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCppCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


/*换弹*/
void AFPSCppCharacter::Reload()
{
	if (FullAmmo == 0)
	{
		bAbleToFire = false;
	}
	if (FullAmmo != 0 && !bIsReloading && PerAmmo != CurrentAmmo)
	{
		if (ReloadMontage)
		{
			PlayAnimMontage(ReloadMontage);
		}

		bAbleToFire = false;
		bIsReloading = true;

		GetCharacterMovement()->MaxWalkSpeed = 270;
		GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AFPSCppCharacter::ReloadFinish, 2.f, false);

		if (CurrentAmmo + FullAmmo < PerAmmo)
		{
			CurrentAmmo += FullAmmo;
			FullAmmo = 0;
		}
		else
		{
			FullAmmo = FullAmmo - PerAmmo + CurrentAmmo;
			CurrentAmmo = PerAmmo;
		}
	}
}

void AFPSCppCharacter::ReloadFinish()
{
	if (!bIsCrouching&&!bIsZooming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600;
	}
	bAbleToFire = true;
	bIsReloading = false;
}

/*手雷*/
void AFPSCppCharacter::Grenade()
{
	if (!bAbleToUseGrenade||GrenadeCount == 0)
	{
		return;
	}
	if (GrenadeClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			const FVector SpawnLocation = GrenadeLocation->GetComponentLocation();
			
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			AGrenade* Grenade = World->SpawnActor<AGrenade>(GrenadeClass, SpawnLocation, SpawnRotation,
			                                                ActorSpawnParams);
			if (Grenade)
			{
				FVector2D ViewportSize;
				FVector ScreenToWorldLoc;
				FVector ScreenToWorldDir;
				GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
				GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(
					ViewportSize.X / 2, ViewportSize.Y / 2, ScreenToWorldLoc, ScreenToWorldDir);
				Grenade->GetSphereComponent()->AddImpulse(ScreenToWorldDir * 30000);
				
				GrenadeCount--;
				bAbleToUseGrenade=false;
				GetWorldTimerManager().SetTimer(GrenadeCoolDownTimerHandle,this,&AFPSCppCharacter::GrenadeCoolDown,5.f,false);
			}
		}
	}
}

void AFPSCppCharacter::GrenadeCoolDown()
{
	bAbleToUseGrenade=true;
}

/*静步*/
void AFPSCppCharacter::Walk()
{
	GetCharacterMovement()->MaxWalkSpeed = 270;
}

void AFPSCppCharacter::StopWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 600;
}

/*蹲*/
void AFPSCppCharacter::OnCrouch()
{
	if (bAbleToCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeed = 270;
		CameraSpringArm->AddLocalOffset(FVector(0.f, 0.f, -40.f));
		bIsCrouching = true;
	}
}

void AFPSCppCharacter::StopCrouch()
{
	if (!bIsZooming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600;
	}
	CameraSpringArm->AddLocalOffset(FVector(0.f, 0.f, 40.f));
	bIsCrouching = false;
}

/*ADS*/
void AFPSCppCharacter::OnZoom()
{
	if (bAbleToZoomIn && ZoomInCamera != nullptr)
	{
		MainCamera = ZoomInCamera;
		bIsZooming = true;
		GetCharacterMovement()->MaxWalkSpeed = 270;
		ZoomInCamera->Activate();
		TPSCameraComponent->Deactivate();
	}
}

void AFPSCppCharacter::StopZoom()
{
	if (bAbleToZoomIn && TPSCameraComponent != nullptr)
	{
		MainCamera = TPSCameraComponent;
		bIsZooming = false;
		if(!bIsReloading)
		{
			GetCharacterMovement()->MaxWalkSpeed = 600;
		}
		TPSCameraComponent->Activate();
	}
}

/*冲刺*/
void AFPSCppCharacter::Run()
{
	if (bAbleToRun)
	{
		bAbleToCrouch = false;
		bAbleToFire = false;
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 900;
	}
}

void AFPSCppCharacter::StopRun()
{
	bAbleToCrouch = true;
	bAbleToFire = true;
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = 600;
}

//准星偏移
float AFPSCppCharacter::FireOffset()
{
	return GetVelocity().Size() / 300.f;
}

float AFPSCppCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
