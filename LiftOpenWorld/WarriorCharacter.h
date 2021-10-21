// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"
#include "WeatherController.h"
#include "FemaleKnightCharacter.h"
#include "WarriorCharacter.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8 {
	SWARD_SLASH		UMETA(DisplayName = "Sward"),
	PUNCH			UMETA(DisplayName = "Fist Punch"),
	KICK			UMETA(DisplayName = "Kick")
};

UENUM(BlueprintType)
enum class EAttackStrength : uint8 {
	LIGHT		UMETA(DisplayName = "Light"),
	MEDIUM			UMETA(DisplayName = "Medium"),
	STRONG			UMETA(DisplayName = "Strong")
};

UCLASS()
class LIFTOPENWORLD_API AWarriorCharacter : public ACharacter
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
	AWarriorCharacter();


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

	/*Slash animation, sound cue and component*/
	UPROPERTY(EditAnywhere, Category = "Weapon")
		bool bIsWeaponEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
		class UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		class USceneComponent* EquippedComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		class USceneComponent* UnEquippedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		class USkeletalMeshComponent* BladeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		class UAnimMontage* SlashAnimMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundCue* SlashSoundCue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* SlashAudioComponent;


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

	void ToggleEquipWeapon();
	void ChangeWeaponAttatchment();

	void Attack();
	void SlashEnd();
	void AttackStart(EAttackType);


	//void AttackEnd(EA);
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

	void SetKeyboardEnabled(bool);
	void PlaySlashSound(bool);
	void AttackHitStart();
	void AttackHitEnd();
	void SetCombo();
	//void PlayCombo();
	void ResetCombo();


	virtual void Landed(const FHitResult& Hit) override;



private:
	void SetSpeedForward(float TargetSpeed);
	void SetSpeedSlide(float TargetSpeed);

	float MaxRunningSpeed = 400;
	float MaxWalkingSpeed = 200;
	float MaxCrouchingSpeed = 100;
	bool IsInAir = false;
	FTimerHandle EquipTimerHandle;
	bool IsKeyboardEnabled = true;
	bool IsInCombo;
	int CurrentComboIndex;
	int MaxComboIndex;
	EAttackType CurrentAttackType;
	UCharacterMovementComponent* CharacterMovement;


	


};
