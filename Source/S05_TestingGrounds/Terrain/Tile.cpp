// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"


// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn)
{
	FVector min = FVector(0, -2000, 0);
	FVector max = FVector(4000, 2000, 0);
	FBox Bounds = FBox(min, max);

	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);

	for(int i = 0; i < NumberToSpawn; i++)
	{
		FVector SpawnPoint = FMath::RandPointInBox(Bounds);
		FVector SpawnRotation = FVector();
		//UE_LOG(LogTemp, Warning, TEXT("Object %d Spawn Point: %s"), i, *SpawnPoint.ToCompactString())
		AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);//, SpawnPoint, SpawnRotation);
		Spawned->SetActorRelativeLocation(SpawnPoint);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

