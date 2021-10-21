// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"
#include "WeatherController.h"
#include "FemaleKnightCharacter.generated.h"

UENUM(BlueprintType)
enum class ELogLevel : uint8 {
	TRACE			UMETA(DisplayName = "Trace"),
	DEBUG			UMETA(DisplayName = "Debug"),
	INFO			UMETA(DisplayName = "Info"),
	WARNING			UMETA(DisplayName = "Warning"),
	ERROR			UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class ELogOutput : uint8 {
	ALL				UMETA(DisplayName = "All levels"),
	OUTPUT_LOG		UMETA(DisplayName = "Output log"),
	SCREEN			UMETA(DisplayName = "Screen")
};

UCLASS()
class LIFTOPENWORLD_API AFemaleKnightCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AFemaleKnightCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
		float SpeedForward;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
		float SpeedSlide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
		bool bIsCrouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* PlayerJumpMontage;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* WalkAudioComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* WalkSoundCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particle, meta = (AllowPrivateAccess = "true"))
		class AWeatherController* ActorWeatherController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Jump();


	void StopJumping();

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

	void Sprint();
	void EndSprint();

	void CrouchStart();
	void CrouchEnd();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void Log(ELogLevel LogLevel, FString Message);
	void Log(ELogLevel LogLevel, FString Message, ELogOutput LogOutput);

	UFUNCTION(BlueprintCallable, Category = Animation)
		float GetSpeedForward();
	
	UFUNCTION(BlueprintCallable, Category = Animation)
		float GetSpeedSlide();

	UFUNCTION(BlueprintCallable, Category = Audio)
		void MoveStart();
	UFUNCTION(BlueprintCallable, Category = Audio)
		void MoveEnd();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool GetIsFalling();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool GetIsCrouch();



	virtual void Landed(const FHitResult& Hit) override;



private:
	void SetSpeedForward(float TargetSpeed);
	void SetSpeedSlide(float TargetSpeed);

	float MaxRunningSpeed = 400;
	float MaxWalkingSpeed = 200;
	float MaxCrouchingSpeed = 100;
	bool IsInAir = false;

	UCharacterMovementComponent* CharacterMovement;
	
};
