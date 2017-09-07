// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius)
{
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for(int i = 0; i < NumberToSpawn; i++)
	{
		FVector SpawnPoint;
		bool found = FindEmptyLocation(SpawnPoint, Radius);
		if(found)
		{
			PlaceActor(ToSpawn, SpawnPoint);
			//UE_LOG(LogTemp, Warning, TEXT("Object %d Spawn Point: %s"), i, *SpawnPoint.ToCompactString())
		}
	}
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FVector min = FVector(0, -2000, 0);
	FVector max = FVector(4000, 2000, 0);
	FBox Bounds = FBox(min, max);

	const int MAX_ATTEMPTS = 100;
	for(int i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if(CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint)
{
		AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);//, SpawnPoint, SpawnRotation);
		Spawned->SetActorRelativeLocation(SpawnPoint);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	//CanSpawnAtLocation(GetActorLocation(), 300);
	//CanSpawnAtLocation(GetActorLocation() + FVector(0, 0, 1000), 300);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;

	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);

	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);

	FColor ResultColor = HasHit ? FColor::Red : FColor::Green;

	//DrawDebugSphere( GetWorld(), Location, Radius, 100, ResultColor, true, 100);
	/*
	if (HasHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Object hit: %s"), *HitResult.GetActor()->GetName())
			UE_LOG(LogTemp, Warning, TEXT("\tComponent hit: %s"), *HitResult.GetComponent()->GetName())
	}
	*/
	DrawDebugCapsule(GetWorld(), Location, 0, Radius, FQuat::Identity, ResultColor, true, 100);

	return !HasHit;
}