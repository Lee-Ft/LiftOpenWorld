// Fill out your copyright notice in the Description page of Project Settings.


#include "FemaleKnightCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"

// Sets default values
AFemaleKnightCharacter::AFemaleKnightCharacter()
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

	//ActorWeatherController = CreateDefaultSubobject<AWeatherController>(TEXT("NOne"));
	//ActorWeatherController->AttachToComponent(FollowCamera, FAttachmentTransformRules::KeepRelativeTransform);
	//ActorWeatherController = CreateDefaultSubobject<AWeatherController>(TEXT("RainParticle"));
	//if (ActorWeatherController != nullptr)
	//{
	//	ActorWeatherController->AttachToComponent(FollowCamera, FAttachmentTransformRules::KeepRelativeTransform);
	//}
	//ActorWeatherController->(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform, TEXT("ParticleSystem"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	static ConstructorHelpers::FObjectFinder<UAnimMontage> PlayerJumpMontageObject(TEXT("AnimMontage'/Game/Character/Human/Animation/FreeWalk/JumpMontage.JumpMontage'"));
	if (PlayerJumpMontageObject.Succeeded())
	{
		PlayerJumpMontage = PlayerJumpMontageObject.Object;

	}
	 
	static ConstructorHelpers::FObjectFinder<USoundCue> WalkSoundCueObject(TEXT("SoundCue'/Game/Character/Human/Audio/WalkSoundCue.WalkSoundCue'"));
	if (WalkSoundCueObject.Succeeded())
	{
		WalkSoundCue = WalkSoundCueObject.Object;
		WalkAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PunchAudioComponent"));
		WalkAudioComponent->SetupAttachment(RootComponent);

	}

}

// Called when the game starts or when spawned
void AFemaleKnightCharacter::BeginPlay()
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

}

// Called every frame
void AFemaleKnightCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void AFemaleKnightCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFemaleKnightCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFemaleKnightCharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFemaleKnightCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFemaleKnightCharacter::EndSprint);
	

	PlayerInputComponent->BindAxis("MoveForward", this, &AFemaleKnightCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFemaleKnightCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFemaleKnightCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFemaleKnightCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AFemaleKnightCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AFemaleKnightCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFemaleKnightCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFemaleKnightCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFemaleKnightCharacter::CrouchEnd);

}


void AFemaleKnightCharacter::OnResetVR()
{
	
}

void AFemaleKnightCharacter::Jump()
{
	Log(ELogLevel::DEBUG, __FUNCTION__);
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
	PlayAnimMontage(PlayerJumpMontage, 1.0f, FName(FString("jump_begin")));
	IsInAir = true;
}



void AFemaleKnightCharacter::StopJumping()
{
	Log(ELogLevel::DEBUG, __FUNCTION__);
	bPressedJump = false;
	ResetJumpState();

}


void AFemaleKnightCharacter::Landed(const FHitResult& Hit)
{
	Log(ELogLevel::DEBUG, __FUNCTION__);
	OnLanded(Hit);

	LandedDelegate.Broadcast(Hit);

	PlayAnimMontage(PlayerJumpMontage, 1.0f, FName(FString("jump_end")));

	IsInAir = false;
}

void AFemaleKnightCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AFemaleKnightCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AFemaleKnightCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFemaleKnightCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFemaleKnightCharacter::MoveForward(float Value)
{
	//Log(ELogLevel::DEBUG, __FUNCTION__);
	if ((Controller != NULL) && (Value != 0.0f))
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

	//float CurrentSpeed = Dot3(Velocity, Forward)
	SetSpeedForward(Dot3(Velocity, Forward));
	SpeedForward = GetSpeedForward();
}


void AFemaleKnightCharacter::MoveRight(float Value)
{
	//Log(ELogLevel::DEBUG, __FUNCTION__);

	if ((Controller != NULL) && (Value != 0.0f))
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



float AFemaleKnightCharacter::GetSpeedForward()
{
	
	return SpeedForward;
}

void AFemaleKnightCharacter::SetSpeedForward(float TargetSpeed)
{
	SpeedForward = TargetSpeed;
}

float AFemaleKnightCharacter::GetSpeedSlide()
{
	return SpeedSlide;
}

void AFemaleKnightCharacter::SetSpeedSlide(float TargetSpeed)
{
	SpeedSlide = TargetSpeed;

}

bool AFemaleKnightCharacter::GetIsFalling()
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


void AFemaleKnightCharacter::Sprint()
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

void AFemaleKnightCharacter::EndSprint()
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

void AFemaleKnightCharacter::MoveStart()
{
	float speed = rand() * 0.1 + 0.5f;
	WalkAudioComponent->Play(speed);
	//WalkAudioComponent->Stop()
}
void AFemaleKnightCharacter::MoveEnd()
{
	//float speed = rand() * 0.01 + 0.5f;
	//WalkAudioComponent->Play(speed);
	WalkAudioComponent->Stop();
}

void AFemaleKnightCharacter::CrouchStart()
{
	Log(ELogLevel::WARNING, __FUNCTION__);
	bIsCrouch = true;
	CharacterMovement->MaxWalkSpeed = MaxCrouchingSpeed;
	//Crouch();
}

void AFemaleKnightCharacter::CrouchEnd()
{
	Log(ELogLevel::WARNING, __FUNCTION__);
	bIsCrouch = false;
	CharacterMovement->MaxWalkSpeed = MaxWalkingSpeed;
	//UnCrouch();
}

bool AFemaleKnightCharacter::GetIsCrouch()
{
	return bIsCrouch;
}

void AFemaleKnightCharacter::Log(ELogLevel Level, FString Message)
{
	Log(Level, Message, ELogOutput::ALL);
}

void AFemaleKnightCharacter::Log(ELogLevel Level, FString Message, ELogOutput LogOutput)
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