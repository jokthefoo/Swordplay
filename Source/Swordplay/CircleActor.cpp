// Fill out your copyright notice in the Description page of Project Settings.

#include "CircleActor.h"

// Sets default values
ACircleActor::ACircleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ACircleActor::HighlightSelection(int sel)
{
	if (sel < 0)
	{
		TestFunc(sel);
	}
	else
	{

	}
}

// Called when the game starts or when spawned
void ACircleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACircleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
