// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"

// Sets default values
AWarriorCharacter::AWarriorCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bIsCrouch = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	static ConstructorHelpers::FObjectFinder<UAnimMontage> PlayerJumpMontageObject(TEXT("AnimMontage'/Game/Character/Warrior/Animation/FreeWalk/JumpMontage.JumpMontage'"));
	if (PlayerJumpMontageObject.Succeeded())
	{
		//Log(ELogLevel::ERROR, FString(TEXT("Load Succeed!!")));
		PlayerJumpMontage = PlayerJumpMontageObject.Object;

	}

	static ConstructorHelpers::FObjectFinder<USoundCue> WalkSoundCueObject(TEXT("SoundCue'/Game/Character/Warrior/Audio/WalkSoundCue.WalkSoundCue'"));
	if (WalkSoundCueObject.Succeeded())
	{
		WalkSoundCue = WalkSoundCueObject.Object;
		WalkAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PunchAudioComponent"));
		WalkAudioComponent->SetupAttachment(RootComponent);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> EquipMontageObject(TEXT("AnimMontage'/Game/Character/Warrior/Animation/Equip/EquipWeaponMontage.EquipWeaponMontage'"));
	if (EquipMontageObject.Succeeded())
	{
		//Log(ELogLevel::ERROR, FString(TEXT("Load Succeed!!")));
		EquipMontage = EquipMontageObject.Object;

	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageObject(TEXT("AnimMontage'/Game/Character/Warrior/Animation/Attack/SlashMontage.SlashMontage'"));
	if (AttackMontageObject.Succeeded())
	{
		SlashAnimMontage = AttackMontageObject.Object;
	}
	bIsWeaponEquipped = true;
	CurrentAttackType = EAttackType::SWARD_SLASH;

	static ConstructorHelpers::FObjectFinder<USoundCue> SlashSoundCueObject(TEXT("SoundCue'/Game/Character/Warrior/Audio/SlashSoundCue.SlashSoundCue'"));
	if (SlashSoundCueObject.Succeeded())
	{
		SlashSoundCue = SlashSoundCueObject.Object;
		SlashAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SlashAudioComponent"));
		SlashAudioComponent->SetupAttachment(RootComponent);
	}
	CurrentComboIndex = 0;
	MaxComboIndex = 3;
	
}

// Called when the game starts or when spawned
void AWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Log(ELogLevel::DEBUG, __FUNCTION__);
	CharacterMovement = GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed = MaxWalkingSpeed;
	CharacterMovement->MaxWalkSpeedCrouched = MaxCrouchingSpeed;
	CharacterMovement->MaxAcceleration = 500.0;
	CharacterMovement->BrakingDecelerationWalking = 500.0;

	WalkAudioComponent->SetSound(WalkSoundCue);

	ActorWeatherController = GetWorld()->SpawnActor<AWeatherController>();
	if (ActorWeatherController != nullptr) {
		ActorWeatherController->AttachToComponent(FollowCamera, FAttachmentTransformRules::KeepRelativeTransform);
	}

	TArray<USceneComponent*> SceneComponentList;
	//TArray<USkeletalMeshComponent*> SkeletalMeshComponentList;
	RootComponent->GetChildrenComponents(true, SceneComponentList);
	//RootComponent->GetChildrenComponents(true, SkeletalMeshComponentList);
	//RootComponent->GetSk
	FString ComponentName;
	for (int b = 0; b < SceneComponentList.Num(); b++)
	{
		SceneComponentList[b]->GetName(ComponentName);
		//Log(ELogLevel::DEBUG, ComponentName);
		if (ComponentName == FString("BladeEquipped"))
		{
			Log(ELogLevel::DEBUG, ComponentName);
			EquippedComponent = SceneComponentList[b];
		}
		if (ComponentName == FString("BladeUnEquipped"))
		{
			Log(ELogLevel::DEBUG, ComponentName);
			UnEquippedComponent = SceneComponentList[b];
		}
		if (ComponentName == FString("SK_Blade_BlackWyrmBlade"))
		{

			BladeComponent = Cast<USkeletalMeshComponent>(SceneComponentList[b]);
			Log(ELogLevel::DEBUG, ComponentName);
		}
	}

	SlashAudioComponent->SetSound(SlashSoundCue);

}

// Called every frame
void AWarriorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void AWarriorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AWarriorCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AWarriorCharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AWarriorCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AWarriorCharacter::EndSprint);


	PlayerInputComponent->BindAxis("MoveForward", this, &AWarriorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWarriorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWarriorCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWarriorCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AWarriorCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AWarriorCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AWarriorCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AWarriorCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AWarriorCharacter::CrouchEnd);

	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AWarriorCharacter::ToggleEquipWeapon);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AWarriorCharacter::Attack);

}


void AWarriorCharacter::OnResetVR()
{

}

void AWarriorCharacter::Jump()
{
	
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
	if (PlayerJumpMontage)
	{
		int Count = PlayerJumpMontage->CompositeSections.Num();
		PlayAnimMontage(PlayerJumpMontage, 1.0f, PlayerJumpMontage->CompositeSections[0].SectionName);
	}
	
	IsInAir = true;	
}


void AWarriorCharacter::StopJumping()
{
	Log(ELogLevel::DEBUG, __FUNCTION__);
	bPressedJump = false;
	ResetJumpState();

}


void AWarriorCharacter::Landed(const FHitResult& Hit)
{
	Log(ELogLevel::DEBUG, __FUNCTION__);
	OnLanded(Hit);

	LandedDelegate.Broadcast(Hit);

	PlayAnimMontage(PlayerJumpMontage, 1.0f, FName(FString("jump_end")));

	IsInAir = false;
}

void AWarriorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AWarriorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AWarriorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWarriorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWarriorCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && IsKeyboardEnabled)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
	FVector Velocity = GetVelocity();
	FVector Forward = GetActorForwardVector();

	SetSpeedForward(Dot3(Velocity, Forward));
	SpeedForward = GetSpeedForward();
}


void AWarriorCharacter::MoveRight(float Value)
{

	if ((Controller != NULL) && (Value != 0.0f) && IsKeyboardEnabled)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);


	}
	FVector Velocity = GetVelocity();
	FVector Slide = GetActorRightVector();

	SetSpeedSlide(Dot3(Velocity, Slide));
	SpeedSlide = GetSpeedSlide();
}



float AWarriorCharacter::GetSpeedForward()
{

	return SpeedForward;
}

void AWarriorCharacter::SetSpeedForward(float TargetSpeed)
{
	SpeedForward = TargetSpeed;
}

float AWarriorCharacter::GetSpeedSlide()
{
	return SpeedSlide;
}

void AWarriorCharacter::SetSpeedSlide(float TargetSpeed)
{
	SpeedSlide = TargetSpeed;

}

bool AWarriorCharacter::GetIsFalling()
{
	//return CharacterMovement->IsFalling();

	if (IsInAir)
	{
		Log(ELogLevel::ERROR, __FUNCTION__);
	}
	else
	{
		Log(ELogLevel::DEBUG, __FUNCTION__);
	}
	return IsInAir;
}


void AWarriorCharacter::Sprint()
{
	Log(ELogLevel::ERROR, __FUNCTION__);
	//MaxSpeedForward = 600;
	if (bIsCrouch)
	{
		CharacterMovement->MaxWalkSpeed = MaxWalkingSpeed;
	}
	else
	{
		CharacterMovement->MaxWalkSpeed = MaxRunningSpeed;
	}


}

void AWarriorCharacter::EndSprint()
{
	Log(ELogLevel::WARNING, __FUNCTION__);
	//MaxSpeedForward = 300;
	if (bIsCrouch)
	{
		CharacterMovement->MaxWalkSpeed = MaxCrouchingSpeed;
	}
	else
	{
		CharacterMovement->MaxWalkSpeed = MaxWalkingSpeed;
	}


}

void AWarriorCharacter::MoveStart()
{
	float speed = rand() * 0.1 + 0.5f;
	WalkAudioComponent->Play(speed);
	//WalkAudioComponent->Stop()
}
void AWarriorCharacter::MoveEnd()
{

	WalkAudioComponent->Stop();
}

void AWarriorCharacter::CrouchStart()
{
	Log(ELogLevel::WARNING, __FUNCTION__);
	bIsCrouch = true;
	CharacterMovement->MaxWalkSpeed = MaxCrouchingSpeed;
}

void AWarriorCharacter::CrouchEnd()
{
	Log(ELogLevel::WARNING, __FUNCTION__);
	bIsCrouch = false;
	CharacterMovement->MaxWalkSpeed = MaxWalkingSpeed;
}

bool AWarriorCharacter::GetIsCrouch()
{
	return bIsCrouch;
}

void AWarriorCharacter::ToggleEquipWeapon()
{
	bIsWeaponEquipped = !bIsWeaponEquipped;
	PlayAnimMontage(EquipMontage, 1.0f, EquipMontage->CompositeSections[0].SectionName);
	
	GetWorld()->GetTimerManager().SetTimer(EquipTimerHandle, this, &AWarriorCharacter::ChangeWeaponAttatchment, 0.7f, false);
	
}

void AWarriorCharacter::ChangeWeaponAttatchment()
{
	if (BladeComponent && EquippedComponent && UnEquippedComponent)
	{
		if (bIsWeaponEquipped)
		{
			Log(ELogLevel::INFO, __FUNCTION__);
			//BladeComponent->SetupAttachment(UnEquippedComponent);
			BladeComponent->AttachToComponent(EquippedComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
		else
		{
			Log(ELogLevel::DEBUG, __FUNCTION__);
			BladeComponent->AttachToComponent(UnEquippedComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	//GetWorldTimerManager().ClearTimer(EquipTimerHandle);
}

void AWarriorCharacter::AttackStart(EAttackType AttackType)
{
	FString StartMontagePoint;
	switch (AttackType)
	{
	case EAttackType::SWARD_SLASH:
		Log(ELogLevel::DEBUG, FString::FromInt(CurrentComboIndex + 1));
		StartMontagePoint = "start_" + FString::FromInt(CurrentComboIndex + 1);
		PlayAnimMontage(SlashAnimMontage, 1.0f, FName(StartMontagePoint));
		break;
	case EAttackType::PUNCH:
		break;
	case EAttackType::KICK:
		break;
	default:
		break;
	}
}

void AWarriorCharacter::Attack()
{

	if (IsKeyboardEnabled)
	{
		AttackStart(CurrentAttackType);
	}
	else
	{
		IsInCombo = true;
	}
}

void AWarriorCharacter::SlashEnd()
{

}

void AWarriorCharacter::SetCombo()
{
	SetKeyboardEnabled(false);

}

void AWarriorCharacter::ResetCombo()
{
	if (IsInCombo)
	{
		IsInCombo = false;
		CurrentComboIndex = (CurrentComboIndex + 1) % MaxComboIndex;
		AttackStart(CurrentAttackType);
	}
	else
	{
		CurrentComboIndex = 0;
		SetKeyboardEnabled(true);
	}
	
}

void AWarriorCharacter::SetKeyboardEnabled(bool bIsEnabled)
{
	IsKeyboardEnabled = bIsEnabled;
}

void AWarriorCharacter::PlaySlashSound(bool bIsPlay)
{
	Log(ELogLevel::DEBUG, __FUNCTION__);
	if (bIsPlay)
	{
		float Speed = rand() * 0.1 + 0.5f;
		SlashAudioComponent->Play(Speed);
	}
	else
	{
		SlashAudioComponent->Stop();
	}
}

void AWarriorCharacter::AttackHitStart()
{
	/*TODO: Attach CollisionBox to Sward, Punch or Legs*/
	PlaySlashSound(true);

}

void AWarriorCharacter::AttackHitEnd()
{
	/*TODOL Attach CollisionBox to Body*/
	PlaySlashSound(false);
}
void AWarriorCharacter::Log(ELogLevel Level, FString Message)
{
	Log(Level, Message, ELogOutput::ALL);
}

void AWarriorCharacter::Log(ELogLevel Level, FString Message, ELogOutput LogOutput)
{
	// only print when screen is selected and the GEngine object is available
	if ((LogOutput == ELogOutput::ALL || LogOutput == ELogOutput::SCREEN) && GEngine)
	{
		// default color
		FColor LogColor = FColor::Cyan;
		// flip the color based on the type
		switch (Level)
		{
		case ELogLevel::TRACE:
			LogColor = FColor::Green;
			break;
		case ELogLevel::DEBUG:
			LogColor = FColor::Cyan;
			break;
		case ELogLevel::INFO:
			LogColor = FColor::White;
			break;
		case ELogLevel::WARNING:
			LogColor = FColor::Yellow;
			break;
		case ELogLevel::ERROR:
			LogColor = FColor::Red;
			break;
		default:
			break;
		}
		// print the message and leave it on screen ( 4.5f controls the duration )
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, LogColor, Message);
	}

	if (LogOutput == ELogOutput::ALL || LogOutput == ELogOutput::OUTPUT_LOG)
	{
		// flip the message type based on error level
		switch (Level)
		{
		case ELogLevel::TRACE:
			UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::DEBUG:
			UE_LOG(LogTemp, Verbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::INFO:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		case ELogLevel::WARNING:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
			break;
		case ELogLevel::ERROR:
			UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
			break;
		default:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		}
	}
}