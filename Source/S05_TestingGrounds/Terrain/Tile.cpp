// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
//#include "DrawDebugHelpers.h"
#include "ActorPool.h"
#include "Runtime/Engine/Classes/AI/Navigation/NavigationSystem.h"
#include "Engine/World.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavigationBoundsOffset = FVector(2000, 0, 0);

	MinExtent = FVector(0, -2000, 0);
	MaxExtent = FVector(4000, 2000, 0);
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
		UE_LOG(LogTemp, Error, TEXT("[%s] Not enough actors in Pool!"), *GetName());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] Checked out {%s}: "), *GetName(), *NavMeshBoundsVolume->GetName());
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	//moving the nav meshes in the editor will automatically build them, doing it in code does not, so build below...
	GetWorld()->GetNavigationSystem()->Build();
	//Find offset for a new nav mesh bounds volume
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	//FRandomProperties RandomProperties (MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);
	FRandomProperties RandomProperties;
	RandomProperties.MinSpawn = MinSpawn;
	RandomProperties.MaxSpawn = MaxSpawn;
	RandomProperties.Radius = Radius;
	RandomProperties.MinScale = MinScale;
	RandomProperties.MaxScale = MaxScale;

	TArray<FSpawnPosition> SpawnPositions = RandomSpawnPositions(RandomProperties);

	for(FSpawnPosition SpawnPosition : SpawnPositions)
	{
		PlaceActor(ToSpawn, SpawnPosition);
	}
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int MinSpawn, int MaxSpawn, float Radius)
{
	FRandomProperties RandomProperties;
	RandomProperties.MinSpawn = MinSpawn;
	RandomProperties.MaxSpawn = MaxSpawn;
	RandomProperties.Radius = Radius;
	RandomProperties.MinScale = 1;
	RandomProperties.MaxScale = 1;

	TArray<FSpawnPosition> SpawnPositions = RandomSpawnPositions(RandomProperties);
	for (FSpawnPosition SpawnPosition : SpawnPositions)
	{
		PlaceAIPawn(ToSpawn, SpawnPosition);
	}
}

TArray<FSpawnPosition> ATile::RandomSpawnPositions(FRandomProperties RandomProperties)
{
	TArray<FSpawnPosition> SpawnPositions;

	int NumberToSpawn = FMath::RandRange(RandomProperties.MinSpawn, RandomProperties.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(RandomProperties.MinScale, RandomProperties.MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, RandomProperties.Radius * SpawnPosition.Scale);
		if (found)
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			//PlaceActor(ToSpawn, SpawnPosition);
			SpawnPositions.Add(SpawnPosition);
			//UE_LOG(LogTemp, Warning, TEXT("Object %d Spawn Point: %s"), i, *TargetSpawnPoint.ToCompactString())
		}
	}

	return SpawnPositions;
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FBox Bounds(MinExtent, MaxExtent);
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

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition SpawnPosition)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
}

void ATile::PlaceAIPawn(TSubclassOf<APawn> ToSpawn, FSpawnPosition SpawnPosition)
{
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
	Spawned->SpawnDefaultController();
	Spawned->Tags.Add(FName("Enemy"));
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