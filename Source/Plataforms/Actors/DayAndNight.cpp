// Fill out your copyright notice in the Description page of Project Settings.


#include "DayAndNight.h"
#include "Engine/DirectionalLight.h"
#include "Misc/OutputDeviceNull.h"
#include "Net/UnrealNetwork.h" ///Para utilizar DOREPLIFETIME


// Sets default values
ADayAndNight::ADayAndNight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	///REPLICATION
	bReplicates = true;
	bAlwaysRelevant = true;
	//bNetUseOwnerRelevancy = true; Esto es cuando tiene como padre un APlayerController
}

// Called when the game starts or when spawned
void ADayAndNight::BeginPlay()
{
	Super::BeginPlay();
	if (!Sun) return;
	//Replico el movimiento del Actor sol para eso utilizo a Rotacion
	if (HasAuthority())//La logica solo la hago en el server
	{
		Rotacion = Sun->GetActorRotation();
	}
	//Al objeto sun Cambiar a movible; 
	Sun->SetMobility(EComponentMobility::Movable);//llamo en el BeginPlay() ya q el sol es un Objeto q se crea x tiempo y una vez creado llamo al SetMobility
}

// Called every frame
void ADayAndNight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		if (!Sun || !SkySphere) return;
		Rotacion.Pitch += Velocidad * DeltaTime;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Rotacion -> %s"), *Rotacion.ToString());
	Sun->SetActorRotation(Rotacion);
	//OjO
	//Esto no se puede ejecutar dentro del Authority porq resetear la posicion del actor se necesita q se llame al UpdateSunDirection tanto en el Serverclient y los clietnes
	FOutputDeviceNull DeviceNull;
	SkySphere->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), DeviceNull, NULL, true);
	//Llamo al metodo del bluepint SkySphere_BP -> UpdateSunDirection
}

///Importante
//Cada vez q el server realiza un cambio en la FRotator Rotacion el servidor manda una replica a todos los clientes.
void ADayAndNight::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADayAndNight, Rotacion);//Params (Class name, PropertyToReplicates)
}
