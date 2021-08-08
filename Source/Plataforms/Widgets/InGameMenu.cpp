// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"


bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return Success;

	if (!BtnCancel) return false;
	BtnCancel->OnClicked.AddDynamic(this, &UInGameMenu::OnClickCancel);

	if (!BtnSalir) return false;
	BtnSalir->OnClicked.AddDynamic(this, &UInGameMenu::OnClickSalir);

	return Success;
}

void UInGameMenu::OnClickCancel()
{
	UE_LOG(LogTemp, Warning, TEXT("Cancel Clicked "));
	if (this->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cancel :V"));
		this->ChangeInputMode(); //Puede ir sin this-> o con ya que es un metodo propio q hereda de la clase padre
	}
}

void UInGameMenu::OnClickSalir()
{
	if (!MenuInterface) return;
	UE_LOG(LogTemp, Warning, TEXT("Salir Clicked "));
	ChangeInputMode();
	MenuInterface->GoToMainMenu();
}