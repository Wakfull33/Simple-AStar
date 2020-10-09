// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTile.h"
#include "TileUserWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values
AGridTile::AGridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	TileUserWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("UserWidget"));

	TileUserWidget->SetWidgetClass(UTileUserWidget::StaticClass());

}

// Called when the game starts or when spawned
void AGridTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridTile::SetTileActorColor(FLinearColor Color)
{
	UMaterialInstanceDynamic* MI = UMaterialInstanceDynamic::Create(StaticMeshComponent->GetMaterial(0), this);
	MI->SetVectorParameterValue(TEXT("TileColor"), FVector(Color));
	StaticMeshComponent->SetMaterial(0, MI);
}
