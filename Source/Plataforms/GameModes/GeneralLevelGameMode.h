#pragma once

#include "CoreMinimal.h"
#include "Plataforms/PlataformsGameMode.h"
#include "GeneralLevelGameMode.generated.h"

UCLASS()
class PLATAFORMS_API AGeneralLevelGameMode : public APlataformsGameMode
{
	GENERATED_BODY()

public:
	//void AlivePlayers(APlayerController* DeadPlayer);

protected:
	///VARIABLES
	TArray<APlayerController*> NewPlayerControllers;

	//FUNCTIONS
	//Se accede cuando se hace un ServerTravel
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	//Se accede cuando se ingresa un nuevo jugador al nivel con este GameMode
	//Pero no se accede cuando se ejecuta un SeamlessTravel
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

private:
	
	void SetPlayerName(APlayerController* PlayerToChangeName);
};
