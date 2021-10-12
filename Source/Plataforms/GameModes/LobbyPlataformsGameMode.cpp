// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlataformsGameMode.h"
#include "Plataforms/GameInstance/GameInstancePlataforms.h"
#include "Plataforms/PlataformsCharacter.h"
#include "GameFramework/PlayerState.h"//Para player State

void ALobbyPlataformsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	Players.Add(NewPlayer);

	if (Players.Num() >= 5)//Num cuenta desde 1
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode lleno %d Jugadores"), Players.Num());
		//ServerTravel();
		//Timer-->https://www.gamedev.tv/courses/635403/lectures/11781024
	}
}

void ALobbyPlataformsGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	APlayerController* MyPlayerController = Cast<APlayerController>(Exiting);

	int Index;
	if (Players.Find(MyPlayerController, Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("Bye men xD -> %s"), *Players[Index]->GetName());
		Players.RemoveAt(Index);
	}
}

void ALobbyPlataformsGameMode::ServerTravel()
{
	//Server Travel
	//Enable Seamless 
	bUseSeamlessTravel = true;
	UGameInstancePlataforms* GameInstance = Cast<UGameInstancePlataforms>(GetGameInstance());
	if (!GameInstance) return;
	//GameInstance->StartSession();
	UWorld* World = GetWorld();
	if (!World) return;
	World->ServerTravel("/Game/Maps/game?listen");
}