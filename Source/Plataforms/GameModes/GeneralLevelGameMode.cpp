// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralLevelGameMode.h"
#include "Plataforms/PlataformsCharacter.h"
#include "GameFramework/PlayerState.h"//Para player State

void AGeneralLevelGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	APlayerController* TempController = Cast<APlayerController>(C);
	NewPlayerControllers.Add(TempController);
	///Steam Names
	for (auto Player : NewPlayerControllers)
	{
		SetPlayerName(Player);
	}
}

void AGeneralLevelGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (NewPlayerControllers.Num() > 0)
	{
		int32 TempIndex;
		if (NewPlayerControllers.Find(NewPlayer, TempIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Que raro!!"));
		}
		else
		{
			NewPlayerControllers.Add(NewPlayer);
			SetPlayerName(NewPlayer);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Algo Anda Mal xD"));
	}
}

void AGeneralLevelGameMode::SetPlayerName(APlayerController* PlayerToChangeName)
{
	APlataformsCharacter* Character = Cast<APlataformsCharacter>(PlayerToChangeName->GetCharacter());
	if (Character)
	{
		FString NombrePlayer = Character->GetPlayerState()->GetPlayerName();
		//LLamo a la funcion de PlataformsCharacter que Set los NickNames cada vez q ingrese un nuevo jugador
		//OJO El GameMode solo se ejecuta en el servidor 
		Character->SetupCharacterName(NombrePlayer);
	}
}

void AGeneralLevelGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	APlayerController* TempPController = Cast<APlayerController>(Exiting);
	if (TempPController)
	{
		int32 Index = 0;
		if (NewPlayerControllers.Find(TempPController, Index))
		{
			UE_LOG(LogTemp, Warning, TEXT("Nos Vemos -> %s"), *NewPlayerControllers[Index]->GetName());
			NewPlayerControllers.RemoveAt(Index);
			UE_LOG(LogTemp, Warning, TEXT("Jugadores -> %i"), NewPlayerControllers.Num());
		}
	}

}

//Solo vale desde el servidor :C
/*void AGeneralLevelGameMode::AlivePlayers(APlayerController* DeadPlayer)
{
	AActor* PlayerStart = FindPlayerStart(DeadPlayer, FString("Start"));
	if (!PlayerStart) return;

	if (NewPlayerControllers.Num() > 0)
	{
		for (auto Player : NewPlayerControllers)
		{
			if (Player== DeadPlayer)//->GetUniqueID()
			{
				UE_LOG(LogTemp, Warning, TEXT("Son iguales %i vs %i !!!!!"), Player->GetUniqueID(), DeadPlayer->GetUniqueID());
				if (PlayerCanRestart(Player))
				{
					//DeadPlayer->UnPossess();
					RestartPlayerAtPlayerStart(Player, PlayerStart);
					APlataformsCharacter* TempCharacter = Cast<APlataformsCharacter>(Player->GetCharacter());
					if (TempCharacter)
					{
						FString NombrePlayer = TempCharacter->GetPlayerState()->GetPlayerName();
						TempCharacter->SetupCharacterName(NombrePlayer);
						//DeadPlayer->Possess(TempCharacter);
					}
				}
				return;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No Son iguales %s vs %s"), *Player->GetName(), *DeadPlayer->GetName());
			}
		}
	}
}
*/