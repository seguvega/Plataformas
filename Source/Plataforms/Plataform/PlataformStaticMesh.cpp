// Fill out your copyright notice in the Description page of Project Settings.


#include "PlataformStaticMesh.h"
#include  "Net/UnrealNetwork.h"

APlataformStaticMesh::APlataformStaticMesh() 
{
	PrimaryActorTick.bCanEverTick = true; ///Activo el tick method en el actor
	SetMobility(EComponentMobility::Movable);///Pongo q pueda moverse este metodo va a un staticMesh o a un Mesh
	///REPLICATION
	bReplicates = true;
	bAlwaysRelevant = true;
}

void APlataformStaticMesh::BeginPlay()
{
	Super::BeginPlay();
	//Si es el servidor replico el APlataformStaticMesh objeto
	if (HasAuthority()) 
	{
		//Consigo la Ubicacion
		LocationI = GetActorLocation();
		GlobalFinalLocation = GetTransform().TransformPosition(FinalLocation);///Local to Global position
	}
	else
	{
		LocationClient = GetActorLocation();///Inicializo esta variable en el cliente
	}
}


void APlataformStaticMesh::SetActivatePlataform()
{
	ActivePlataform += 1;
	if (ActivePlataform >= 2) 
	{
		Velocidad *= 3;
	}
}

void APlataformStaticMesh::SetDeactivatePlataform()
{
	if (ActivePlataform <= 0) return; //Early return
	ActivePlataform -= 1;
	if (ActivePlataform == 1)
	{
		Velocidad /= 3;
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
				//Si la distancia entre la ubicacion actual y eL punto final de la plataforma es <= (0.F + DeltaTime * Velocidad) la plataforma de la vuelta 
				if (FVector::Dist(Location, GlobalFinalLocation) <= (0.F + DeltaTime * Velocidad))
				{
					//UE_LOG(LogTemp, Error, TEXT("TAS CERCA"));
					Mov = false;
				}
				Direction = (GlobalFinalLocation - LocationI).GetSafeNormal();//Final - Inicial
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
			LocationClient = Location;//Mando la nueva ubicacion de las plataformas
			SetActorLocation(Location);
		}
	}
	else //Esto ejecutaria solo el cliente siempre
	{
		SetActorLocation(LocationClient);
		//UE_LOG(LogTemp, Warning, TEXT("Eres el cliente")); 
		//Types of data for UE_LOG
		//BOOL -> %s
		//STRING -> %s  --- *YourVariable
		//INTEGER -> %d
		//FLOAT -> %f
	}
}

///Se ejecuta cuando el servidor detecta cambios en la variable y envia a todos los clientes una replica de esa variable
void APlataformStaticMesh::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlataformStaticMesh, LocationClient);
}