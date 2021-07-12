// Fill out your copyright notice in the Description page of Project Settings.


#include "PlataformStaticMesh.h"

APlataformStaticMesh::APlataformStaticMesh() 
{
	PrimaryActorTick.bCanEverTick = true; ///Activo el tick method en el actor
	SetMobility(EComponentMobility::Movable);///Pongo q pueda moverse este metodo va a un staticMesh o a un Mesh
}

void APlataformStaticMesh::BeginPlay()
{
	Super::BeginPlay();
	//Si es el servidor replico el APlataformStaticMesh objeto
	if (HasAuthority()) 
	{
		SetReplicates(true);//Para hacer q este objeto se replique pero este solo funciona en el server
		SetReplicateMovement(true);
		//Consigo la Ubicacion
		LocationI = GetActorLocation();
		GlobalFinalLocation = GetTransform().TransformPosition(FinalLocation);///Local to Global position
	}
}

void APlataformStaticMesh::SetActivatePlataform()
{
	ActivePlataform += 1;
	if (ActivePlataform >= 2) 
	{
		Velocidad *= 2;
	}
}

void APlataformStaticMesh::SetDeactivatePlataform()
{
	if (ActivePlataform <= 0) return; //Early return
	ActivePlataform -= 1;
	if (ActivePlataform == 1)
	{
		Velocidad /= 2;
	}
}


void APlataformStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///Mover (solo el server)
	if (HasAuthority())
	{
		if (ActivePlataform > 0)
		{
			FVector Location = GetActorLocation();
			FVector Direction;
			/*UE_LOG(LogTemp, Warning, TEXT("Dis %f"), FVector::Dist(Location, GlobalFinalLocation));
			UE_LOG(LogTemp, Warning, TEXT("min %f"), (0.F + DeltaTime * Velocidad));*/
			//manera 2 https://www.gamedev.tv/courses/635403/lectures/11780589 min 9:40
			//	Comprar los tamaños de los vectores (GlobalFinalLocation - LocationI).Size() con  (Location - LocationI).Size() si el 2
			//  es mas grande significa q la paltaforma se paso del puntoFinal
			if (Mov)
			{
				//Si la distancia entre EL punto final y la ubicacion actual de la plataforma es <= (0.F + DeltaTime * Velocidad) la plataforma de la vuelta 
				if (FVector::Dist(Location, GlobalFinalLocation) <= (0.F + DeltaTime * Velocidad))
				{
					//UE_LOG(LogTemp, Error, TEXT("TAS CERCA"));
					Mov = false;
				}
				Direction = (GlobalFinalLocation - LocationI).GetSafeNormal();
			}
			else
			{
				if (FVector::Dist(Location, LocationI) <= (0.F + DeltaTime * Velocidad))
				{
					//UE_LOG(LogTemp, Error, TEXT("TAS CERCA"));
					Mov = true;
				}
				Direction = (LocationI - GlobalFinalLocation).GetSafeNormal();
			}
			Location += Velocidad * DeltaTime * Direction; ///Mover en X

			SetActorLocation(Location);
		}
	}
	/*else //Esto ejecutaria el cliente siempre
	{
		//UE_LOG(LogTemp, Warning, TEXT("Eres el cliente")); 
		//Types of data for UE_LOG
		//BOOL -> %s
		//STRING -> %s  --- *YourVariable
		//INTEGER -> %d
		//FLOAT -> %f
		
	}*/
}