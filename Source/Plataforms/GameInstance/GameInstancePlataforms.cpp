#include "GameInstancePlataforms.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"//Para encontrar archivos
#include "Blueprint/UserWidget.h"//Para utilizar el modulo UMG en el .Build.cs
#include "Plataforms/Widgets/MainMenuWidget.h"
#include "Plataforms/Widgets/InGameMenu.h"
#include "Plataforms/Widgets/SeverList.h"
#include "OnlineSessionSettings.h"//SessionSettings
#include "Interfaces/OnlineIdentityInterface.h"//Para llamar a los metodos de GetIdentityInterface()
#include "GameFramework/PlayerState.h"//Para acceder al Id y el nombre del jugador

///Variable Global --- solo para test
//const static FName SESSION_NAME = TEXT("My Session Game");

UGameInstancePlataforms::UGameInstancePlataforms(const FObjectInitializer& ObjectInitializer)
{
	///UE_LOG(LogTemp, Error, TEXT("Hello GameInstance Constructor!!!"));
	//Inicializo
	PlayerId = 0;  ///No se porq es 0 pero creo q hace referencia al número de jugadores locales (2 jugando en la misma pc seria 0 y 1)
	///Utilizo las funciones que usan como parametro de ID el FUniqueNetId envez de un uint32

	//Encuentro el UUSerWidget para conseguir la Clase no el objeto OJO
	static ConstructorHelpers::FClassFinder<UUserWidget> WBPMenu(TEXT("/Game/MenuSystem/MenuWBP"));
	if (!WBPMenu.Class) return;
	MenuWidget = WBPMenu.Class;

	//Encuentro el UUSerWidget InGameMenu
	static ConstructorHelpers::FClassFinder<UUserWidget> WBPInGameMenu(TEXT("/Game/MenuSystem/InGameMenuWBP"));
	if (!WBPInGameMenu.Class) return;
	InGameMenuWidget = WBPInGameMenu.Class;
	//UE_LOG(LogTemp, Warning, TEXT("Clase PB: %s"), *MenuWidget->GetName());

	//Encuentro el ServerList
	static ConstructorHelpers::FClassFinder<UUserWidget> WPBServerList(TEXT("/Game/MenuSystem/ServerListWPB"));
	if (!WPBServerList.Class) return;
	ServerList = WPBServerList.Class;
}

void UGameInstancePlataforms::Init()
{
	//Llamo al OSS(OnlineSubsystem) en el metodo Init el cual se ejecuta cuando se da a jugar es decir se llama multiples veces
	IOnlineSubsystem* OnlineSubs = IOnlineSubsystem::Get();
	if (OnlineSubs)
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem: %s"), *OnlineSubs->GetSubsystemName().ToString());
		//Clase IOnlineSessionPtr 
		SessionPtr = OnlineSubs->GetSessionInterface();
		if (SessionPtr.IsValid())//Retorna un true si encuentra una session 
		{
			//se ejecuta cuando haya una session, hace algo muy parecido al Bind .AddDynamic() 
			//Que cuando la session se haya creado poder saberlo si es creo bien o no y poder ejecutar una funcion();
			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UGameInstancePlataforms::OnSessionResp);//Se ejecuta cuando acabe de crear la session
			SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UGameInstancePlataforms::OnEndSessionResp);//Se ejecuta cuando acabe de eliminar la session
			SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UGameInstancePlataforms::OnFindSession);//Se ejecuta cuando acabe de buscar Sessiones activas
			SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UGameInstancePlataforms::OnJoinSessionComplete);
		}
		//Clase OnlineIdentityInterface
		NetPlayerId = OnlineSubs->GetIdentityInterface()->GetUniquePlayerId(0); //Validar .IsValid()
		if (NetPlayerId.IsValid())
		{
			PlayerName = FName(OnlineSubs->GetIdentityInterface()->GetPlayerNickname(*NetPlayerId));//Para acceder al TSharedPtr necesito explicar q es un puntero * 
			UE_LOG(LogTemp, Error, TEXT("PlayerNickname: %s"), *PlayerName.ToString());
			UE_LOG(LogTemp, Error, TEXT("IdentityInterface Player Id: %s"), *NetPlayerId->ToString());
		}
		//Engine para cuando hay problemas de conección
		if (GEngine != nullptr)
		{
			GEngine->OnNetworkFailure().AddUObject(this, &UGameInstancePlataforms::BroadcastNetworkFailure);//Se ejecuta cuando hay errores de conección
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No se encontro un OnlineSubsystem"));
	}
}

void UGameInstancePlataforms::LoadMenu()
{
	if (!MenuWidget) return;
	Menu = CreateWidget<UMainMenuWidget>(this, MenuWidget);///Se puede crear UUSerWidget desde el GameInstance, playerController y World
	Menu->SetUpWidget();
	//Importante seteo la interface para q desde el widget pueda llamar al metodo host gracias a la interface
	Menu->SetMenuInterface(this);
}

void UGameInstancePlataforms::LoadInGameMenu()
{
	if (!InGameMenuWidget) return;
	InGameMenu = CreateWidget<UInGameMenu>(this, InGameMenuWidget);
	InGameMenu->SetUpWidget();
	InGameMenu->SetMenuInterface(this);
}

void UGameInstancePlataforms::ResumeInGameMenu() 
{
	if (!InGameMenu) return;
	InGameMenu->OnClickCancel();//Es un metodo publico de UInGameMenu
}

void UGameInstancePlataforms::GoToMainMenu()
{
	///Sacar al jugador de la Session
	if (SessionPtr.IsValid())
	{
		//Host
		if (SessionPtr->GetNamedSession(PlayerName))
		{
			SessionPtr->DestroySession(PlayerName);//Respuesta Asyncrona
		}
		else if (SessionSearch.IsValid() && NetPlayerId.IsValid() && UserSelectedIndex.IsSet())//Cliente
		{
			UE_LOG(LogTemp, Error, TEXT("Saliendo de ->%s"), *SessionSearch->SearchResults[UserSelectedIndex.GetValue()].Session.OwningUserName);
			SessionPtr->UnregisterPlayer(FName(SessionSearch->SearchResults[UserSelectedIndex.GetValue()].Session.OwningUserName), *NetPlayerId);
			///Para usar el UnregisterPlayer necesito primero registrarles -> RegisterPlayer() a los jugadores
		}

		//Client Travel
		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (!PlayerController) return;
		PlayerController->ClientTravel("/Game/MenuSystem/MenuWidget", ETravelType::TRAVEL_Absolute);
	}
}

void UGameInstancePlataforms::Host()
{
	if (SessionPtr.IsValid() && NetPlayerId.IsValid())//Retorna un true si encuentra una session y un NetPlayerId
	{
		if (SessionPtr->GetNamedSession(PlayerName)) //Si existe la session se destruye y si no existe se crea
		{
			SessionPtr->DestroySession(PlayerName);//Respuesta Asyncrona
		}
		else
		{
			CreateSession();
		}
	}
}

void UGameInstancePlataforms::CreateSession()
{
	FOnlineSessionSettings SessionSettings;
	//Veo si Existe OSubsystemSteam o OSubsystemNull
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		SessionSettings.bIsLANMatch = true;
	}
	else
	{
		SessionSettings.bIsLANMatch = false;
	}
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true; //joining in progress is allowed or not
	SessionSettings.bAllowInvites = true; //Acepte Invitaciones
	SessionSettings.bAllowJoinViaPresence = true; //No se q hace
	//Config para usar Steam Lobbys
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true; ///Usa lobbys
	UE_LOG(LogTemp, Error, TEXT("Creando Session %s ..."), *PlayerName.ToString());
	//Ctrl + Click en CreateSession y ir a bool FOnlineSessionSteam para ver el codigo de ese metodo OJO
	SessionPtr->CreateSession(*NetPlayerId, PlayerName, SessionSettings);//Creo la session --> Respuesta Asyncrona
}

void UGameInstancePlataforms::OnSessionResp(FName SessionName, bool Resp)
{
	if (!Resp)
	{
		UE_LOG(LogTemp, Error, TEXT("Error al Crear la Session %s"), *SessionName.ToString());
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Session %s Creada"), *SessionName.ToString());
	if (!Menu) return;
	UEngine* Engine = GetEngine();
	if (!Engine) return;
	Engine->AddOnScreenDebugMessage(0, 10, FColor::Green, TEXT("Hosting"));
	//Restablesco el Input
	Menu->ChangeInputMode();
	//Server Travel
	UWorld* World = GetWorld();
	if (!World) return;
	World->ServerTravel("/Game/Maps/lobby?listen");
	
}

void UGameInstancePlataforms::Join(uint32 Index)
{
	if (!Menu) return;
	Menu->ChangeInputMode();
	if (!SessionPtr.IsValid()) return;
	if (!NetPlayerId.IsValid()) return;
	if (!SessionSearch.IsValid()) return;
	UserSelectedIndex = Index;//Set del valor del intex en mi TOptional<>
	UE_LOG(LogTemp, Error, TEXT("Entrando a --->%s"), *SessionSearch->SearchResults[Index].Session.OwningUserName);
	SessionPtr->JoinSession(*NetPlayerId, FName(SessionSearch->SearchResults[Index].Session.OwningUserName), SessionSearch->SearchResults[Index]);//Respuesta Asyncrona / SearchResults[Index] Es un TArray
}

void UGameInstancePlataforms::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionPtr.IsValid()) return;
	FString Address;
	//Retorna un parametro cuando tiene el simbolo & ya q hace referencia a ese valor y lo carga de data
	//GetResolvedConnectString tiene multiples metodos que tienen diferentes parametros, para saber q parametros tiene siempre dar click en el metodo para ver el código
	//Aunq parese q se maneja el puerto automaticamente Port 7777 or 7778
	if (!SessionPtr->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Error, TEXT("No se pudo obtener una IP"));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!Engine) return;
	Engine->AddOnScreenDebugMessage(0, 10, FColor::Green, FString::Printf(TEXT("Joining: %s"), *Address));
	//Client Travel
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!PlayerController) return;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

	///Registro al Cliente en la Session al parecer por defaul ejecuta -> RegisterLocalPlayers el cual no registra bien creo
	SessionPtr->RegisterPlayer(SessionName, *NetPlayerId, true);
	//Restablesco el Input
	if (!Menu) return;
	Menu->ChangeInputMode();
}

void UGameInstancePlataforms::BuscarServerList()
{
	//FindSession
	SessionSearch = MakeShareable(new FOnlineSessionSearch());///Me sirve para convertir un puntero a una ref
	if (SessionPtr.IsValid() && SessionSearch.IsValid() && NetPlayerId.IsValid())//Si es un puntero valido
	{
		//SessionSearch->bIsLanQuery = true; //Sirve para buscar unicamente partidas lan

		//Steam Lobby
		//QuetySettings es un FOnlineSearchSettings
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->MaxSearchResults = 150;
		SessionPtr->FindSessions(*NetPlayerId, SessionSearch.ToSharedRef());//Convierto mi SessionSearch en un Ref, SessionSearch se carga con la sessiones q encuentra a la vez da una respuesta Asyncrona
		UE_LOG(LogTemp, Error, TEXT("Buscando Sessiones"));
	}
}

void UGameInstancePlataforms::OnFindSession(bool Resp)
{
	if (Resp)
	{
		if (SessionSearch.IsValid())
		{
			uint32 Index = 0;//para saber el index
			TArray<FOnlineSessionSearchResult> Sessiones = SessionSearch->SearchResults;
			for (auto MySession : Sessiones)//For in range
			{
				UE_LOG(LogTemp, Error, TEXT("Session encontrada-> %s"), *MySession.Session.OwningUserName);
				if (!ServerList || !Menu) return;
				USeverList* List = CreateWidget<USeverList>(this, ServerList);
				List->SetUp(Menu, Index);//Mando la clase Menu ya que ahi se van a añadir los USeverList y el index
				//Creo la FStruct para enviar toda la info posible al USeverList
				FServerData NewStruc;
				NewStruc.Name = MySession.GetSessionIdStr();
				NewStruc.MaxPlayers = MySession.Session.SessionSettings.NumPublicConnections;//Maximo de jugadores
				NewStruc.CurrentPlayers = (NewStruc.MaxPlayers - MySession.Session.NumOpenPublicConnections);//Maximo - Los JugadoresActuales
				NewStruc.UserName = MySession.Session.OwningUserName;// == GetIdentityInterface()->GetPlayerNickname(*)
				NewStruc.Ping = MySession.PingInMs;
				List->SetText(NewStruc);
				Menu->AgregarServidores(List);
				Index += 1;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("Buscar Session Terminado..!!"));
	}
}

void UGameInstancePlataforms::OnEndSessionResp(FName SessionName, bool Resp)
{
	//Si ya se elimino la session creo otra
	if (Resp)
	{
		//CreateSession();
		UE_LOG(LogTemp, Error, TEXT("Session ELIMINADA"));
	}
}

/*
void UGameInstancePlataforms::StartSession()
{
	if (SessionPtr.IsValid())
	{
		//Compruebo si la session esta creada y si esta la marca como session iniciada y ya no aparece al buscar///PROBAR -> igual ya no sale la partida
		SessionPtr->StartSession(PlayerName);
	}
}*/

void UGameInstancePlataforms::Exit()
{
	UE_LOG(LogTemp, Error, TEXT("Salir!!!!!!!!!!!!!"));
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!PlayerController) return;
	PlayerController->ConsoleCommand(FString("Exit"));//Para reinicial el nivel RestarLevel
}

void UGameInstancePlataforms::BroadcastNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UEngine* Engine = GetEngine();
	if (!Engine) return;
	Engine->AddOnScreenDebugMessage(0, 10, FColor::Green, FString::Printf(TEXT("Error: %s"), *ErrorString));
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!PlayerController) return;
	PlayerController->ClientTravel("/Game/MenuSystem/MenuWidget", ETravelType::TRAVEL_Absolute);
}

void UGameInstancePlataforms::chat(FString msg)
{
	UEngine* Engine = GetEngine();
	if (!Engine) return;
	Engine->AddOnScreenDebugMessage(0, 10, FColor::Green, FString::Printf(TEXT("Desconocido: %s"), *msg));
}