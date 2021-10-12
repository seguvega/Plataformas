// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PlataformStaticMesh.generated.h"

/**
 Aqui Van las clases extra
 */

UCLASS()
class PLATAFORMS_API APlataformStaticMesh : public AStaticMeshActor
{
	GENERATED_BODY()

	APlataformStaticMesh();
protected:

	virtual void BeginPlay() override;

public:
	///VARIABLES
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Componentes")//No necesita, meta = (AllowPrivateAccess = "true")
	int32 Velocidad = 50;
	///meta = (MakeEditWidget = "true") //Me permite editar el F Vector en el Blueprint
	//Esta Location es relativa osea local, al APlataformStaticMesh Objeto y no es la Location Global
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Componentes", meta = (MakeEditWidget = "true"))
	FVector FinalLocation;
	
	///FUNCIONES
	//Set Activate and Desactivate
	void SetActivatePlataform();
	
	void SetDeactivatePlataform();
	
private:
	///VARIABLES
	bool Mov = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Componentes", meta = (AllowPrivateAccess = "true"))
	int32 ActivePlataform = 1;

	
	FVector LocationI;

	UPROPERTY(Replicated) ///añadir -> "Net/UnrealNetwork.h" en el .cpp 
	FVector LocationClient;

	FVector GlobalFinalLocation;

	///FUNCIONES
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
