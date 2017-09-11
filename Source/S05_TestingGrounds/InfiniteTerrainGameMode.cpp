// Fill out your copyright notice in the Description page of Project Settings.

#include "InfiniteTerrainGameMode.h"
#include "AI/Navigation/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "ActorPool.h"

AInfiniteTerrainGameMode::AInfiniteTerrainGameMode()
{
	NavMeshBoundsVolumePool = CreateDefaultSubobject<UActorPool>(FName("Nav Mesh Bounds Volume Pool"));
}


void AInfiniteTerrainGameMode::PopulateBoundsVolumePool()
{
	//TActorIterator<ANavMeshBoundsVolume> 
	auto VolumeIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	while (VolumeIterator) // If there is another Actor to Iterate over, then we can use this...
	{
		AddToPool(*VolumeIterator);
		++VolumeIterator; //must be prefix ++	//"advance" the iterator, move to next...
	}

}

void AInfiniteTerrainGameMode::AddToPool(ANavMeshBoundsVolume *VolumeToAdd)
{
	//UE_LOG(LogTemp, Warning, TEXT("Found Nav Mesh Bounds Volume: %s"), *VolumeToAdd->GetName())
	NavMeshBoundsVolumePool->Add(VolumeToAdd);
}