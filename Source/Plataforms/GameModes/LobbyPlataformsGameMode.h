// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plataforms/PlataformsGameMode.h"
#include "LobbyPlataformsGameMode.generated.h"

UCLASS()
class PLATAFORMS_API ALobbyPlataformsGameMode : public APlataformsGameMode
{
	GENERATED_BODY()

public:
	///FUNCTONS
	//Método de la clase padre AGameModeBase Called after a successful login. (To acces father metods go to the FatherClass)
	virtual void PostLogin(APlayerController* NewPlayer) override;

	//Called when a Controller with a PlayerState leaves the game or is destroyed 
	virtual void Logout(AController* Exiting) override;

protected:
	///VARIABLES
	TArray<APlayerController*> Players;

private:
	///FUNCIONES
	UFUNCTION(BlueprintCallable, Category = "Game")//LLamarle desde el BLueprint
	void ServerTravel();
};
