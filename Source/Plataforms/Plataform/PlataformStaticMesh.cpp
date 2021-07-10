// Fill out your copyright notice in the Description page of Project Settings.


#include "PlataformStaticMesh.h"

APlataformStaticMesh::APlataformStaticMesh() 
{
	PrimaryActorTick.bCanEverTick = true; ///Activo el tick method en el actor
	SetMobility(EComponentMobility::Movable);///Pongo q pueda moverse
}

void APlataformStaticMesh::BeginPlay()
{
	Super::BeginPlay();
	//Si es el servidor replico el APlataformStaticMesh objeto
	if (HasAuthority()) 
	{
		SetReplicates(true);//Para hacer q este objeto se replique pero este solo funciona en el server
		SetReplicateMovement(true);
	}
}

void APlataformStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///Mover (solo el server)
	if (HasAuthority())
	{
		//Consigo la Ubicacion
		FVector Location = GetActorLocation();
		Location += FVector(Velocidad * DeltaTime, 0, 0); ///Mover en X
		SetActorLocation(Location);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Eres el cliente"));
		/*
		BOOL -> %s
		STRING -> %s  --- *YourVariable
		INTEGER -> %d
		FLOAT -> %f
		*/
	}

}