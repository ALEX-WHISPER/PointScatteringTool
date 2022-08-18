// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleScatter.h"
#include "Math/Halton.h"
#include "NavigationSystem.h"

// Sets default values
ASimpleScatter::ASimpleScatter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

TArray<FVector> ASimpleScatter::ScatterActor(AActor* InActor, const float InScatterIntensity, bool bIgnoreZ)
{
	TArray<FVector> OutPoints;
	if (!InActor)
	{
		return OutPoints;
	}

	FVector LocalBoundsCenter, LocalBoundsExtent;
	GetActorLocalBounds(InActor, LocalBoundsCenter, LocalBoundsExtent);

	if (LocalBoundsExtent.IsZero())
	{
		return OutPoints;
	}

	const uint8 ScatterBaseX = 2;
	const uint8 ScatterBaseY = 3;
	const uint8 ScatterBaseZ = 5;
	const int ScatterCount = LocalBoundsExtent.Size() * InScatterIntensity;

	for (int i = 0; i < ScatterCount; i++)
	{
		const float HaltonX = Halton(i, ScatterBaseX);
		const float HaltonY = Halton(i, ScatterBaseY);
		const float HaltonZ = Halton(i, ScatterBaseZ);
		const FVector HaltonSeqVector = FVector(HaltonX,HaltonY,HaltonZ) - FVector(0.5f);
		
		FVector ScatteredPointLocation = LocalBoundsCenter;
		ScatteredPointLocation += InActor->GetActorForwardVector() * 2 * LocalBoundsExtent.X * HaltonSeqVector.X; // move along the forward direction
		ScatteredPointLocation += InActor->GetActorRightVector() * 2 * LocalBoundsExtent.Y * HaltonSeqVector.Y;	// move along the right direction
		
		if (!bIgnoreZ)
		{
			ScatteredPointLocation += InActor->GetActorUpVector() * 2 * LocalBoundsExtent.Z * HaltonSeqVector.Z;	// move along the up direction
		}
		else
		{
			ScatteredPointLocation.Z = LocalBoundsCenter.Z;
		}
		
		OutPoints.Add(ScatteredPointLocation);
	}
	return OutPoints;
}

TArray<FVector> ASimpleScatter::ConvertScatteredPointsIntoNavmeshPoints(TArray<FVector> InScatteredPoints, ARecastNavMesh* InRecastNavmesh, FVector InQueryExtent)
{
	TArray<FVector> OutPointsOnNavmesh;
	
	if (!InRecastNavmesh)
	{
		return OutPointsOnNavmesh;
	}

	if (!(InQueryExtent.Size() > 0))
	{
		return OutPointsOnNavmesh;
	}

	for (FVector ScatteredPoint: InScatteredPoints)
	{
		TArray<FNavLocation> OutLocations;
		InRecastNavmesh->ProjectPointMulti(ScatteredPoint, OutLocations, InQueryExtent, ScatteredPoint.Z - InQueryExtent.Z, ScatteredPoint.Z + InQueryExtent.Z);

		if (OutLocations.Num() > 0)
		{
			OutPointsOnNavmesh.Add(OutLocations[0].Location);
		}
	}

	return OutPointsOnNavmesh;
}

void ASimpleScatter::GetRootComponentBounds(AActor* InActor, FVector& OutCenter, FVector& OutExtent)
{
	if (InActor)
	{
		FBox Box = InActor->GetRootComponent()->Bounds.GetBox();
		OutCenter = Box.GetCenter();
		OutExtent = Box.GetExtent();	
	}
}

void ASimpleScatter::GetRootComponentLocalBounds(AActor* InActor, FVector& OutCenter, FVector& OutExtent)
{
	if (InActor)
	{
		FBox Box = InActor->GetRootComponent()->GetLocalBounds().GetBox();
        OutCenter = Box.GetCenter();
        OutExtent = Box.GetExtent();	
	}
}

void ASimpleScatter::GetActorLocalBounds(const AActor* InActor, FVector& OutCenter, FVector& OutExtent)
{
	if (!InActor)
	{
		return;
	}

	if (!InActor->GetRootComponent())
	{
		return;
	}
	
	FBox BoundingBox = InActor->GetRootComponent()->GetLocalBounds().GetBox();
	BoundingBox.GetCenterAndExtents(OutCenter,OutExtent);
	
	OutCenter += InActor->GetRootComponent()->GetComponentLocation();
	OutExtent *= InActor->GetRootComponent()->GetRelativeScale3D();
}