// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlataformsCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"//Para encontrar la animación
#include "Components/TextRenderComponent.h"//Para el UTextRenderComponent
#include "Plataforms/GameModes/GeneralLevelGameMode.h"//GameMode
#include "Kismet/KismetSystemLibrary.h" //Para dibujar en la pantalla DrawDebugString()

//////////////////////////////////////////////////////////////////////////
// APlataformsCharacter

APlataformsCharacter::APlataformsCharacter()
{
	//Anim Roll
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimRoll(TEXT("/Game/Mannequin/Animations/Roll"));
	if (AnimRoll.Object)
	{
		RollMontage = AnimRoll.Object;
		//UE_LOG(LogTemp, Warning, TEXT("Encontrado: %s"), *RollMontage->GetName());
	}
	IsRolling = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 94.0f);
	//Hago visible el CapsuleComponent
	GetCapsuleComponent()->SetHiddenInGame(false);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//Salud del Character
	Salud = 100.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 460.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->AirControl = 0.4f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create UTextRender
	PlayerNick = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Nick Name"));
	PlayerNick->SetupAttachment(RootComponent);
	PlayerNick->SetRelativeLocation(FVector(0.f, 0.f, 105.f));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	///Replication --La clase ACharacter ya replica
	//bReplicates = true;

	//PlayerNick init
	SteamNick = "";

	///USkeletalMeshComponent
	FVector TempRelativeLocation = GetMesh()->GetRelativeLocation();
	TempRelativeLocation.Z = 93.f;
	GetMesh()->SetRelativeLocation(TempRelativeLocation);
}

// Input
void APlataformsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlataformsCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlataformsCharacter::StopSprint);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlataformsCharacter::Roll);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlataformsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlataformsCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &APlataformsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlataformsCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &APlataformsCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APlataformsCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APlataformsCharacter::OnResetVR);
}

void APlataformsCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Creo una Bind a la function OnMontageEnded
	//OjO 
	//OnMontageEnd() tiene q se UFUNCTION
	//Esto se puede hacer a todas las funciones Delegate
	//Se ejecuta tanto en el servidor como en el cliente
	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &APlataformsCharacter::OnMontageEnd);
	this->OnDestroyed.AddDynamic(this, &APlataformsCharacter::OnDestroy);
}


void APlataformsCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlataformsCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

///Roll
void APlataformsCharacter::Roll()
{
	if (!GetMovementComponent()->IsFalling())
	{
		if (!IsRolling)
		{
			IsRolling = true;
			if (!RollMontage) return;
			PlayAnimOnServer();
		}
	}
}

void APlataformsCharacter::PlayAnimOnServer_Implementation()
{
	IsRolling = true;
	PlayAnimEverywhere();//NetMulticast ejecuta en el cliente y en el server
}

void APlataformsCharacter::PlayAnimEverywhere_Implementation()
{
	Rolling();
}

void APlataformsCharacter::Rolling()
{
	RollSetupServer(true);
	if (GetCharacterMovement()->MaxWalkSpeed > 300)
	{
		PlayAnimMontage(RollMontage, 2.f);
	}
	else
	{
		PlayAnimMontage(RollMontage);
	}
}

///Se ejecuta en
/// El Server (Autority)
/// El cliente (AutonomousProxy)
/// El Replicated (SimulatedProxy) 
void APlataformsCharacter::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage && !RollMontage) return;
	if (Montage->GetName() == RollMontage->GetName())//Compruebo q es Roll vs Roll
	{
		//UE_LOG(LogTemp, Warning, TEXT("Si o No %i"), GetMesh()->GetAnimInstance()->Montage_IsPlaying(RollMontage));
		IsRolling = false;
		///LLamo RollCapsuleReduce para q ejecute el aumento el UCapsuleComponent
		if (IsLocallyControlled())
		{
			RollSetupServer(false);
		}
		FString LocalRole;
		//GetLocalRole() -- Veo si el APlataformsCharacter es Authority o si es AutonomousProxy o SimulatedProxy
		UEnum::GetValueAsString(GetLocalRole(), LocalRole);
		UE_LOG(LogTemp, Warning, TEXT(" %s"), *LocalRole);
	}
}

void APlataformsCharacter::RollSetupServer_Implementation(bool StartOrEnd)
{
	//Reduzco o incremento el CapsuleComponent Nota--> En el BP seleccionar CapsuleComponent y Desactivar Hidden in Game
	UCapsuleComponent* TempCapsule = GetCapsuleComponent();
	if (!TempCapsule) return;
	FVector PosiCharacter = GetMesh()->GetRelativeLocation();
	float Height = TempCapsule->GetUnscaledCapsuleHalfHeight();
	if (StartOrEnd)
	{
		Height /= 2;
		//Pongo el Mismo radio y hago - alta (Height) la capsula
		TempCapsule->SetCapsuleSize(TempCapsule->GetUnscaledCapsuleRadius(), Height);

		//Subo el Mesh
		PosiCharacter.Z += 40.f;
		GetMesh()->SetRelativeLocation(PosiCharacter);
		RollSetupClient(Height, PosiCharacter);//Para q Haga lo mismo del server pero en el cliente
	}
	else
	{
		Height *= 2;
		//Pongo el Mismo radio y hago + alta (Height) la capsula
		TempCapsule->SetCapsuleSize(TempCapsule->GetUnscaledCapsuleRadius(), Height);
		
		//Bajo el Mesh
		PosiCharacter.Z = -93.f;//No sale restandole -37.f sale -182 envez de -93 
		GetMesh()->SetRelativeLocation(PosiCharacter);
		RollSetupClient(Height, PosiCharacter);
	}
}

void APlataformsCharacter::RollSetupClient_Implementation(float Height, FVector CharacterMeshLocation)
{
	UCapsuleComponent* ClientTempCapsule = GetCapsuleComponent();
	if (!ClientTempCapsule) return;
	ClientTempCapsule->SetCapsuleSize(ClientTempCapsule->GetUnscaledCapsuleRadius(), Height);
	GetMesh()->SetRelativeLocation(CharacterMeshLocation);
}

void APlataformsCharacter::Sprint()
{
	if (GetCharacterMovement()->MaxWalkSpeed <= 300)//Si la velocidad es < o igual a 300
	{
		///OjO Primero seteo el MaxSpeed en el cliente posteriormente actualizo esa velocidad en el servidor
		float WalkSpeed = GetCharacterMovement()->MaxWalkSpeed * 2.f;//600
		SprintOnServer(WalkSpeed);//Se llama desde el Cliente y Ejecuta en el Server
	}
}

void APlataformsCharacter::StopSprint()
{
	if (GetCharacterMovement()->MaxWalkSpeed >=600)//Doble de MaxWalkSpeed -> 300
	{
		float WalkSpeed = GetCharacterMovement()->MaxWalkSpeed / 2.f;
		SprintOnServer(WalkSpeed);
	}
}

void APlataformsCharacter::SprintOnServer_Implementation(float WalkSpeed)
{
	//Se Ejecuta en el servidor
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	SprintOnClient(WalkSpeed);//Llamo desde el server y se ejecuta en el cliente
}

void APlataformsCharacter::SprintOnClient_Implementation(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlataformsCharacter::SetupCharacterName(FString PlayerName)
{
	//SteamNickName
	SteamNick = PlayerName;
	PlayerNick->SetText(FText::FromString(SteamNick));
	UE_LOG(LogTemp, Warning, TEXT("Servidor Nombre: %s"), *SteamNick);
}

void APlataformsCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlataformsCharacter, SteamNick);
}

void APlataformsCharacter::OnNameChange()
{
	PlayerNick->SetText(FText::FromString(SteamNick));
}

void APlataformsCharacter::OnDestroy(AActor* DestroyedActor)
{
	//Se ejecuta en el server y en el cliente, pero me aseguro q solo se ejecute en el server
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Destruido: %s"), *DestroyedActor->GetName());
	}
}

bool APlataformsCharacter::Respawn(FVector Location)
{
	return TeleportTo(Location, GetActorRotation(), false, false);
}

//Touchscreem
void APlataformsCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void APlataformsCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

//Analog -> PS o Xbox
void APlataformsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlataformsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlataformsCharacter::OnResetVR()
{
	// If Plataforms is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Plataforms.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APlataformsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we are a client we might have received an update from the server.
	const bool bIsClient = (GetLocalRole() == ROLE_AutonomousProxy && IsNetMode(NM_Client));


	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		
	}


	FString LocalRole;
	//GetLocalRole() -- Veo si el APlataformsCharacter es Authority o si es AutonomousProxy o SimulatedProxy
	UEnum::GetValueAsString(GetLocalRole(), LocalRole);
	UKismetSystemLibrary::DrawDebugString(GetWorld(), FVector(0.f, 0.f, 100.f), LocalRole, this, FLinearColor::Black, 0.f);
}
