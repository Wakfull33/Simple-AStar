// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Constants.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS()
class A_STAR_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground")
	TEnumAsByte<FGroundTypes> GroundType = FGroundTypes::GP_EASY;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
};
