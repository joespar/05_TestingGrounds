// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPool.h"

// Sets default values for this component's properties
UActorPool::UActorPool()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

AActor* UActorPool::Checkout()
{
	//UE_LOG(LogTemp, Warning, TEXT("[%s] Checkout Pool"), *(GetName()) );
	if(Pool.Num() == 0)
	{
		return nullptr;
	}
	return Pool.Pop();
}

void UActorPool::Return(AActor* ActorToReturn)
{
	/*if(ActorToReturn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] actor returned null argument."), *GetName());
		return;
	}*/
	Add(ActorToReturn);
}

void UActorPool::Add(AActor* ActorToAdd)
{
	/*if (ActorToAdd == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] actor returned null argument."), *GetName());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] actor returned to Pool: {%s}"), *GetName(), *ActorToAdd->GetName());
	*/
	Pool.Push(ActorToAdd);
}