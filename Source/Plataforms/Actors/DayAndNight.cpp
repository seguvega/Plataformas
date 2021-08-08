// Fill out your copyright notice in the Description page of Project Settings.


#include "DayAndNight.h"
#include "Engine/DirectionalLight.h"
#include "Misc/OutputDeviceNull.h"


// Sets default values
ADayAndNight::ADayAndNight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADayAndNight::BeginPlay()
{
	Super::BeginPlay();
	if (!Sun) return;
	Sun->SetMobility(EComponentMobility::Movable);//Al objeto sun Cambiar a mobible;

	///Solo el server ejecuta esto
	if (HasAuthority())
	{
		//Replico el movimiento del sol
		Sun->SetReplicates(true);
		Sun->SetReplicateMovement(true);
		Rotacion = Sun->GetActorRotation();
	}
}

// Called every frame
void ADayAndNight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		if (!Sun || !SkySphere) return;
		Rotacion.Pitch += Velocidad * DeltaTime;
		Sun->SetActorRotation(Rotacion);
	}
	//Esto no se puede ejecutar dentro del Authority
	FOutputDeviceNull DeviceNull;
	SkySphere->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), DeviceNull, NULL, true);
	//Llamo al metodo del bluepint SkySphere_BP -> UpdateSunDirection
}

