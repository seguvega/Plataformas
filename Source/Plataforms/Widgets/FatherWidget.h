// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Plataforms/Widgets/Interfaces/MenuInterface.h" //Cargo mi clase interface -> IMenuInterface
#include "FatherWidget.generated.h"

class IMenuInterface;

UCLASS()
class PLATAFORMS_API UFatherWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	///FUNCIONES
	//Setar la property of class MenuInterface
	void SetMenuInterface(IMenuInterface* MI);

	void SetUpWidget();

	void ChangeInputMode();

protected:
	///VARIABLES
	//Interface
	IMenuInterface* MenuInterface;//Es protected porq quiero q los hijos puedan acceder

	///FUNCIONES
	virtual bool Initialize() override;///Revisar el override
};
