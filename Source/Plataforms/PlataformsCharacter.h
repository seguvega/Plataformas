// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"//RPC
#include "PlataformsCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputComponent;
class UAnimMontage;
class UTextRenderComponent;

UCLASS(config=Game)
class APlataformsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Name, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* PlayerNick;

public:
	APlataformsCharacter();

	//Steam Name
	UPROPERTY(ReplicatedUsing = OnNameChange);
	FString SteamNick;

	//Salud
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Salud")
	float Salud;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	//Roll
	UAnimMontage* RollMontage;

	bool IsRolling;

	///Set the name in Server Site
	void SetupCharacterName(FString PlayerName);

	///ReplicatedUsing SteamNick
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	void OnNameChange();

	//Respawn llamo desde el ATeleporterZ
	bool Respawn(FVector Location);

	/** Returns CameraBoom subobject **/
	FORCEINLINE  USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE  UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	//Sprint
	void Sprint();

	void StopSprint();

	//Roll
	void Roll();

	void Rolling();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	
	///REPLICATED using RPC "Net/UnrealNetwork.h"
	UFUNCTION(Server, reliable)
	void PlayAnimOnServer();
	virtual void PlayAnimOnServer_Implementation();

	UFUNCTION(NetMulticast, reliable)
	void PlayAnimEverywhere();
	virtual void PlayAnimEverywhere_Implementation();

	UFUNCTION(Server, reliable)
	void RollSetupServer(bool StartOrEnd);
	virtual void RollSetupServer_Implementation(bool StartOrEnd);

	UFUNCTION(Client, reliable)
	void RollSetupClient(float Height, FVector CharacterMeshLocation);
	virtual void RollSetupClient_Implementation(float Height, FVector CharacterMeshLocation);

	UFUNCTION(Server, reliable)
	void SprintOnServer(float WalkSpeed);
	virtual void SprintOnServer_Implementation(float WalkSpeed);

	UFUNCTION(Client, reliable)
	void SprintOnClient(float WalkSpeed);
	virtual void SprintOnClient_Implementation(float WalkSpeed);

private:
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDestroy(AActor* DestroyedActor);

	virtual void Tick(float DeltaTime) override;
};