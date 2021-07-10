// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PlataformStaticMesh.generated.h"

/**
 * 
 */
UCLASS()
class PLATAFORMS_API APlataformStaticMesh : public AStaticMeshActor
{
	GENERATED_BODY()

	APlataformStaticMesh();
protected:
	virtual void BeginPlay() override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Velocidad", meta = (AllowPrivateAccess = "true"))
	int32 Velocidad = 50;
	

	virtual void Tick(float DeltaTime) override;
};
