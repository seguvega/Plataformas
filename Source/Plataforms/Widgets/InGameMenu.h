// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
#include "Plataforms/Widgets/FatherWidget.h"//Cargo a la clase padre
#include "InGameMenu.generated.h"

class UButton;
class UFatherWidget;//Clase Padre Widget

UCLASS()
class PLATAFORMS_API UInGameMenu : public UFatherWidget
{
	GENERATED_BODY()

public:
	///VARIABLES
	UPROPERTY(meta = (BindWidget))
	UButton* BtnCancel;

	UPROPERTY(meta = (BindWidget))
	UButton* BtnSalir;

	///FUNCIONES
	UFUNCTION()
	void OnClickCancel();

protected:
	virtual bool Initialize() override;

private:
	///FUNCIONES
	UFUNCTION()
	void OnClickSalir();
};
