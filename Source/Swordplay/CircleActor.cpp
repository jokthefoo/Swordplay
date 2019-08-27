// Fill out your copyright notice in the Description page of Project Settings.

#include "CircleActor.h"
#include "SwordplayCharacter.h"

// Sets default values
ACircleActor::ACircleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ACircleActor::PassCharRef(ASwordplayCharacter * cRef, bool bgmode)
{
	charRef = cRef;
	BoyGirlMode = bgmode;
	CircleCreated();
}

void ACircleActor::HighlightSelection(int sel)
{
	HighlightFunc(sel+1);
}

void ACircleActor::MakeSelection()
{
	SelectionMade();
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

void ACircleActor::CircleEnd()
{
	GLog->Log("Circle ending");
	if (charRef != NULL)
	{
		charRef->CircleEnded();
	}
}
