// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "BallProjectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
AGun::AGun()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}


// Called when the game starts
void AGun::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::OnFire()
{
	//UE_LOG(LogTemp, Warning, TEXT("init AGun::OnFire()"))
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<ABallProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation1P != nullptr && AnimInstance1P != nullptr)
	{
		// Get the animation object for the arms mesh
		AnimInstance1P->Montage_Play(FireAnimation1P, 1.f);
	}
	//UE_LOG(LogTemp, Warning, TEXT(" end AGun::OnFire()"))
	if (FireAnimation3P != nullptr && AnimInstance3P != nullptr)
	{
		// Get the animation object for the arms mesh
		AnimInstance3P->Montage_Play(FireAnimation3P, 1.f);
	}
}

