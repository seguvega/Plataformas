#include "FatherWidget.h"

bool UFatherWidget::Initialize()
{
	bool Success = Super::Initialize();
	return Success;
}

void UFatherWidget::SetMenuInterface(IMenuInterface* MI)
{
	UE_LOG(LogTemp, Error, TEXT("------------------Interface Set------------------"));
	MenuInterface = MI;//Set el valor de MenuInterface property of class
}

void UFatherWidget::SetUpWidget()
{
	//Añado el widget al viewport
	this->AddToViewport();
	//PlayerController para poder dar click en el viewport
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;
	//Pongo el InputMode como solo UI  para manipular el viewport
	FInputModeUIOnly MyInputMode;
	MyInputMode.SetWidgetToFocus(this->TakeWidget());//Necesito convertit mi UUserWidget(UMainMenuWidget) a SWidget "Menu->TakeWidget"
	MyInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);//try LockAlways
	//Guardo la configuracion de MyInputMode
	PlayerController->SetInputMode(MyInputMode);
	PlayerController->bShowMouseCursor = true;
}

void UFatherWidget::ChangeInputMode()
{
	//Remuevo el Widget del viewport
	this->RemoveFromViewport();
	APlayerController* TempPlayerController = GetWorld()->GetFirstPlayerController();
	if (!TempPlayerController) return;
	FInputModeGameOnly MyGameMode;
	TempPlayerController->SetInputMode(MyGameMode);
	TempPlayerController->bShowMouseCursor = false;
}