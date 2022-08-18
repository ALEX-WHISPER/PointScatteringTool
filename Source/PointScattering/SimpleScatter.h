// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem/Public/NavMesh/RecastNavMesh.h"
#include "SimpleScatter.generated.h"

UCLASS()
class POINTSCATTERING_API ASimpleScatter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleScatter();
	
	UFUNCTION(BlueprintCallable, Category=Scatter)
	TArray<FVector> ScatterActor(AActor* InActor, const float InScatterIntensity, bool bIgnoreZ);
	
	UFUNCTION(BlueprintCallable, Category=Scatter)
	TArray<FVector> ConvertScatteredPointsIntoNavmeshPoints(TArray<FVector> InScatteredPoints, ARecastNavMesh* InRecastNavmesh, FVector InQueryExtent); //TSubclassOf<UNavigationQueryFilter> FilterClass);

	UFUNCTION(BlueprintCallable, Category=Debug)
	void GetRootComponentBounds(AActor* InActor, FVector& OutCenter, FVector& OutExtent);

	UFUNCTION(BlueprintCallable, Category=Debug)
	void GetRootComponentLocalBounds(AActor* InActor, FVector& OutCenter, FVector& OutExtent);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void GetActorLocalBounds(const AActor* InActor, FVector& OutCenter, FVector& OutExtent);
};
