// Fill out your copyright notice in the Description page of Project Settings.


#include "FingerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h" // GetWorld()->GetTimeSeconds() ����� ���� ����

// Sets default values
AFingerCharacter::AFingerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Score = 0;
	bIsLeftPressed = false;
	bIsRightPressed = false;
	LastLeftClickTime = -1.0f; // �ʱⰪ -1 (��ȿ���� ���� �ð�)
	LastRightClickTime = -1.0f; // �ʱⰪ -1 (��ȿ���� ���� �ð�)
	RhythmWindowTolerance = 0.2f; // �⺻ Ÿ�̹� ��� ����
	bCanMove = false; // �ʱ⿡�� �̵� ��Ȱ��ȭ

	// --- ���ӵ� �ý��� ���� �ʱ�ȭ ---
	DefaultMaxWalkSpeed = 100.0f;
	ConsecutiveRhythmHits = 0;
	MaxConsecutiveRhythmHits = 15; // 12�� ���� �� �ִ� �ӵ�
	MinSpeedMultiplier = 1.0f;     // �ּ� �ӵ� ����
	MaxSpeedMultiplier = 3.6f;     // �ִ� �ӵ� ����
	CurrentMovementSpeedMultiplier = MinSpeedMultiplier;

	// --- Ŭ���� ���� ���� ���� �ʱ�ȭ ---
	MaxScore = 3; 
	bIsSpecialCoinCollected = false;
	LevelMaxTime = 180.0f; // 3�� (180��)���� ����
	TimeRemaining = LevelMaxTime;

	bStar1Achieved = false;
	bStar2Achieved = false;
	bStar3Achieved = false;

	// --- ������ ���� ���� �ʱ�ȭ ---
	DefaultJumpZVelocity = 800.0f; // BeginPlay���� ���� ���� ������ ����
	MaxJumpZVelocityMultiplier = 1.5f; // �ִ� �ӵ��� �� �������� 1.5�� �����ϵ��� ���� (���� ����)

	bIsLevelCleared = false; 
	bHasGameEnded = false;   
}

// Called when the game starts or when spawned
void AFingerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// �������Ʈ���� ������ ���� MaxWalkSpeed�� ����
	DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// CharacterMovementComponent�� JumpZVelocity �⺻���� ����
	DefaultJumpZVelocity = GetCharacterMovement()->JumpZVelocity;

	// --- ���� Ÿ�̸� ���� ---
    // 1�ʸ��� UpdateGameTimer �Լ� ȣ��
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AFingerCharacter::UpdateGameTimer, 1.0f, true);

	// �ʱ� �ӵ� ���� ����
	UpdateMovementSpeed();
}

// Called every frame
void AFingerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFingerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &AFingerCharacter::OnLeftClick);
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &AFingerCharacter::OnRightClick);

	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &AFingerCharacter::OnLeftRelease);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &AFingerCharacter::OnRightRelease);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFingerCharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFingerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("LookRight", this, &APawn::AddControllerYawInput);
}


void AFingerCharacter::OnLeftClick()
{
	bIsLeftPressed = true;
	LastLeftClickTime = UGameplayStatics::GetTimeSeconds(GetWorld()); // Ŭ�� �ð� ���
}

void AFingerCharacter::OnRightClick()
{
	bIsRightPressed = true;
	LastRightClickTime = UGameplayStatics::GetTimeSeconds(GetWorld()); // Ŭ�� �ð� ���
}

void AFingerCharacter::OnLeftRelease()
{
	bIsLeftPressed = false;
}

void AFingerCharacter::OnRightRelease()
{
	bIsRightPressed = false;
}

void AFingerCharacter::OnLeftFootDown()
{
	float CurrentTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	if (bIsLeftPressed && (CurrentTime - LastLeftClickTime <= RhythmWindowTolerance))
	{
		bCanMove = true;
		LastLeftClickTime = -1.0f;

		// --- ���� ���� �� ���ӵ� ���� ---
		ConsecutiveRhythmHits = FMath::Min(ConsecutiveRhythmHits + 1, MaxConsecutiveRhythmHits);
		UpdateMovementSpeed(); // �ӵ� ������Ʈ
		//UE_LOG(LogTemp, Warning, TEXT("Left Foot Down + Left Click SUCCESS! Hits: %d"), ConsecutiveRhythmHits);
	}
	else
	{
		bCanMove = false;
		// --- ���� ���� �� ���ӵ� ���� ---
		ConsecutiveRhythmHits = 0; // ���� ���� Ƚ�� �ʱ�ȭ
		UpdateMovementSpeed(); // �ӵ� �ʱ�ȭ
		//UE_LOG(LogTemp, Warning, TEXT("Left Foot Down but Left Click FAILED. Stopping. Hits Reset."));
	}
}

void AFingerCharacter::OnRightFootDown()
{
	float CurrentTime = UGameplayStatics::GetTimeSeconds(GetWorld());
	if (bIsRightPressed && (CurrentTime - LastRightClickTime <= RhythmWindowTolerance))
	{
		bCanMove = true;
		LastRightClickTime = -1.0f;

		// --- ���� ���� �� ���ӵ� ���� ---
		ConsecutiveRhythmHits = FMath::Min(ConsecutiveRhythmHits + 1, MaxConsecutiveRhythmHits);
		UpdateMovementSpeed(); // �ӵ� ������Ʈ
		//UE_LOG(LogTemp, Warning, TEXT("Right Foot Down + Right Click SUCCESS! Hits: %d"), ConsecutiveRhythmHits);
	}
	else
	{
		bCanMove = false;
		// --- ���� ���� �� ���ӵ� ���� ---
		ConsecutiveRhythmHits = 0; // ���� ���� Ƚ�� �ʱ�ȭ
		UpdateMovementSpeed(); // �ӵ� �ʱ�ȭ
		//UE_LOG(LogTemp, Warning, TEXT("Right Foot Down but Right Click FAILED. Stopping. Hits Reset."));
	}
}


// --- ���ο� ���ӵ� ��� �� ���� �Լ� ---
void AFingerCharacter::UpdateMovementSpeed()
{
	// ��� ������ ����� SpeedMultipliers �迭�� ���� ���
	int32 Index = FMath::Clamp(ConsecutiveRhythmHits, 0, SpeedMultipliers.Num() - 1);

	CurrentMovementSpeedMultiplier = SpeedMultipliers[Index];

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * CurrentMovementSpeedMultiplier;
		
		// 2. ������(JumpZVelocity) ������Ʈ (�ӵ� ������ ���)
	   // CurrentMovementSpeedMultiplier�� 1.0 (�ּ�)�� �� DefaultJumpZVelocity ����
	   // CurrentMovementSpeedMultiplier�� MaxSpeedMultiplier (3.6)�� �� JumpZVelocity�� DefaultJumpZVelocity * MaxJumpZVelocityMultiplier (1.5)�� �ǵ��� ���� ����

	   // �ӵ� ������ MinSpeedMultiplier(1.0) ~ MaxSpeedMultiplier(3.6) ���� ������
	   // ���� ������ 1.0 ~ MaxJumpZVelocityMultiplier(1.5) ���� ���� ����

		float JumpMultiplier = FMath::Lerp(1.0f, MaxJumpZVelocityMultiplier,
			(CurrentMovementSpeedMultiplier - MinSpeedMultiplier) / (MaxSpeedMultiplier - MinSpeedMultiplier));

		// MinSpeedMultiplier�� MaxSpeedMultiplier�� ������ ��� (��: �� �� 1.0�� ���) ������ 0 ����
		if (FMath::IsNearlyEqual(MaxSpeedMultiplier, MinSpeedMultiplier))
		{
			JumpMultiplier = 1.0f; // ��ȭ ����
		}
		else
		{
			JumpMultiplier = FMath::Lerp(1.0f, MaxJumpZVelocityMultiplier,
				(CurrentMovementSpeedMultiplier - MinSpeedMultiplier) / (MaxSpeedMultiplier - MinSpeedMultiplier));
		}

		GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity * JumpMultiplier;

		/*UE_LOG(LogTemp, Warning, TEXT("Updated MaxWalkSpeed: %.1f (x%.1f), JumpZ: %.1f (x%.1f), Hits: %d"),
			GetCharacterMovement()->MaxWalkSpeed, CurrentMovementSpeedMultiplier,
			GetCharacterMovement()->JumpZVelocity, JumpMultiplier, ConsecutiveRhythmHits);*/
	}
}

void AFingerCharacter::MoveForward(float AxisValue)
{
	// bCanMove�� true�̰�, �̵� �Է��� ���� ���� ���� �̵��� ���
	if ((Controller != nullptr) && (AxisValue != 0.0f) && bCanMove)
	{
		AddMovementInput(GetActorForwardVector() * AxisValue);
	}
	
}

void AFingerCharacter::Jump()
{
	Super::Jump(); // ACharacter�� �⺻ Jump ����� ȣ���մϴ�.
	bJumpInputPressed = true; // ���� �Է��� �������� ǥ��
}

void AFingerCharacter::StopJumping()
{
	Super::StopJumping();
}

void AFingerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit); // ACharacter�� �⺻ Landed ����� ȣ���մϴ�.
	bJumpInputPressed = false; // ���������Ƿ� ���� �Է� ���¸� ���� 
}

// ���� �ý���
void AFingerCharacter::AddScore(int32 ScoreToAdd)
{
	Score++;
	UE_LOG(LogTemp, Warning, TEXT("Current Score: %d"), Score); // ������� ���� �α� ���

	// ��� ������ ȹ������ �� Ŭ���� ���� Ȯ��
	if (Score >= MaxScore)
	{
		CheckClearConditions();
	}
}
// --- ����� ���� ȹ�� ���¸� �����ϴ� �Լ� ���� ---
void AFingerCharacter::SetSpecialCoinCollected(bool bCollected)
{
	bIsSpecialCoinCollected = bCollected;
	if (bIsSpecialCoinCollected)
	{
		bStar3Achieved = true; // ����� ���� ȹ�� �� �� 3 �޼�
		UE_LOG(LogTemp, Warning, TEXT("Special Coin Collected! Star 3 Achieved!"));
		// ����� ���� ȹ�� �� �ٷ� Ŭ���� ���� Ȯ�� (���� ����)
		// CheckClearConditions(); 
	}
}
// --- Ÿ�̸� ������Ʈ �Լ� ���� ---
void AFingerCharacter::UpdateGameTimer()
{
	if (TimeRemaining > 0)
	{
		TimeRemaining -= 1.0f; // 1�� ����
		if (TimeRemaining < 0)
		{
			TimeRemaining = 0;
		}
		UE_LOG(LogTemp, Warning, TEXT("Time Remaining: %.1f"), TimeRemaining);

		// �ð��� 0�� �Ǹ� ���� ���� �Ǵ� Ư�� �̺�Ʈ �߻�
		if (TimeRemaining <= 0 && Score < MaxScore)
		{
			// �ð� �ʰ��� ���� ���� ���� ����
			bIsLevelCleared = false;
			bHasGameEnded = true;

			UE_LOG(LogTemp, Warning, TEXT("Time's Up! Game Over."));
			GetWorldTimerManager().ClearTimer(GameTimerHandle); // Ÿ�̸� ����

			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
		}
	}
}


// --- Ŭ���� ���� Ȯ�� �Լ� ���� ---
void AFingerCharacter::CheckClearConditions()
{
	if (bHasGameEnded)
	{
		return;
	}

	// �� 1: ���� ��� ȹ��
	if (Score >= MaxScore)
	{
		bIsLevelCleared = true;
		bStar1Achieved = true;
		UE_LOG(LogTemp, Warning, TEXT("Star 1 Achieved: All Coins Collected!"));
	}

	// �� 2: ���� ��� ȹ�� �� ���� �ð��� 1��(60��) �̻�
	if (bStar1Achieved && TimeRemaining >= 60.0f)
	{
		bStar2Achieved = true;
		UE_LOG(LogTemp, Warning, TEXT("Star 2 Achieved: Time Remaining >= 1 minute!"));
	}

	// �� 3: ����� ���� ȹ�� (AddScore���� �̹� ó���� �� ����)
	// bIsSpecialCoinCollected�� ����� ���� ȹ�� �� �ٷ� true�� �����ǹǷ�,
	// ���⼭�� ���������� Ȯ�θ� �մϴ�.
	if (bIsSpecialCoinCollected)
	{
		bStar3Achieved = true;
		UE_LOG(LogTemp, Warning, TEXT("Star 3 Achieved: Special Coin Collected!"));
	}

	// ��� �� ȹ�� ���� ���
	UE_LOG(LogTemp, Warning, TEXT("--- Clear Results ---"));
	UE_LOG(LogTemp, Warning, TEXT("Star 1: %s"), (bStar1Achieved ? TEXT("YES") : TEXT("NO")));
	UE_LOG(LogTemp, Warning, TEXT("Star 2: %s"), (bStar2Achieved ? TEXT("YES") : TEXT("NO")));
	UE_LOG(LogTemp, Warning, TEXT("Star 3: %s"), (bStar3Achieved ? TEXT("YES") : TEXT("NO")));
	UE_LOG(LogTemp, Warning, TEXT("---------------------"));

	// ��� ������ ȹ���ߴٸ� Ÿ�̸� ����
	GetWorldTimerManager().ClearTimer(GameTimerHandle);

	// TODO: ���� ���� �� �߰� ó�� (��: ĳ���� �̵� ����, �ִϸ��̼� ���� ��)
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}

