// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "DrawDebugHelpers.h"
#include "ActorPool.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATile::SetPool(UActorPool* InPool)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Setting Pool %s"), *(this->GetName()), *(InPool->GetName()));
	Pool = InPool;

	PositionNavMeshBoundsVolume();
}

void ATile::PositionNavMeshBoundsVolume()
{
	NavMeshBoundsVolume = Pool->Checkout();
	if(NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough actors in Pool!"));
		return;
	}
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation());
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for(size_t i = 0; i < NumberToSpawn; i++)
	{
		FVector SpawnPoint;
		float RandomScale = FMath::RandRange(MinScale, MaxScale);
		bool found = FindEmptyLocation(SpawnPoint, Radius * RandomScale);
		if(found)
		{
			float RandomRotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPoint, RandomRotation, RandomScale);
			//UE_LOG(LogTemp, Warning, TEXT("Object %d Spawn Point: %s"), i, *TargetSpawnPoint.ToCompactString())
		}
	}
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FVector Min(0, -2000, 0);
	FVector Max(4000, 2000, 0);
	FBox Bounds(Min, Max);
	const int MAX_ATTEMPTS = 100;
	for(size_t i = 0; i < MAX_ATTEMPTS; i++)
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

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float Rotation, float Scale)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPoint);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0, Rotation, 0));
	Spawned->SetActorScale3D(FVector(Scale));
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	//TActorIterator<AActor> ActorIterator = TActorIterator<AActor>(GetWorld());
	//while(ActorIterator) // If there is another Actor to Iterate over, then we can use this...
	//{
	//	AActor* FoundActor = *ActorIterator;
	//	UE_LOG(LogTemp, Warning, TEXT("Found Actor: s%"), *FoundActor->GetName())
	//	//"advance" the iterator, move to next...
	//	++ActorIterator; //must be prefix ++
	//}
}


void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//UE_LOG(LogTemp, Warning, TEXT("[%s] EndPlay"), *GetName())
	Pool->Return(NavMeshBoundsVolume);
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

	//FColor ResultColor = HasHit ? FColor::Red : FColor::Green;
	//DrawDebugSphere( GetWorld(), Location, Radius, 100, ResultColor, true, 100);
	/*
	if (HasHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Object hit: %s"), *HitResult.GetActor()->GetName())
			UE_LOG(LogTemp, Warning, TEXT("\tComponent hit: %s"), *HitResult.GetComponent()->GetName())
	}
	*/
	//DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, ResultColor, true, 100);

	return !HasHit;
}