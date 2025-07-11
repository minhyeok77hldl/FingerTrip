// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FingerCharacter.generated.h"

UCLASS()
class FINGERTRIP_API AFingerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFingerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveForward(float AxisValue);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bJumpInputPressed = false; // ���� �Է��� ������ �� true


    UFUNCTION()
    void OnLeftClick();

    UFUNCTION()
    void OnRightClick();

    UFUNCTION()
    void OnLeftRelease();

    UFUNCTION()
    void OnRightRelease();

    UPROPERTY(VisibleAnywhere)
    bool bIsLeftPressed;

    UPROPERTY(VisibleAnywhere)
    bool bIsRightPressed;

    // ��Ŭ���� ���������� ���� ���� �ð� (��)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    float LastLeftClickTime;

    // ��Ŭ���� ���������� ���� ���� �ð� (��)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
    float LastRightClickTime;

    // ���� ���� Ÿ�̹� ��� ���� (��)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rhythm")
    float RhythmWindowTolerance;

    // ���� �̵��� ���Ǵ��� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bCanMove;

    // �ִϸ��̼� Notify���� ȣ��� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void OnLeftFootDown();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void OnRightFootDown();



    // ACharacter�� ���� ���� �Լ� �������̵�
    virtual void Jump() override;
    virtual void StopJumping() override;
    virtual void Landed(const FHitResult& Hit) override;

    // --- ���ӵ� �ý����� ���� ���ο� ���� �� �Լ� ---
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Rhythm")
    int32 ConsecutiveRhythmHits; // �������� ������ ������ Ƚ��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rhythm")
    int32 MaxConsecutiveRhythmHits; // �ӵ��� �ִ�� �����ϴ� ���� ���� Ƚ�� 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rhythm")
    float MinSpeedMultiplier; // �ּ� �ӵ� ��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rhythm")
    float MaxSpeedMultiplier; // �ִ� �ӵ� ���� 

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Rhythm")
    float CurrentMovementSpeedMultiplier; // ���� ����Ǵ� �̵� �ӵ� ����

    // �ӵ��� ������Ʈ�ϴ� �Լ�
    void UpdateMovementSpeed();


private:
    float DefaultMaxWalkSpeed; // ĳ������ ���� �ִ� �ȱ� �ӵ� �����
    // **���ο� ��� ����: �ӵ� ������ ������ ��� �迭**
    const TArray<float> SpeedMultipliers = {
        1.0f, 1.0f, 1.2f, 1.4f, 1.6f, 1.8f,
        2.0f, 2.2f, 2.4f, 2.6f, 2.8f, 3.0f,
        3.2f, 3.4f, 3.6f
    };

    // --- ���� Ŭ���� �� ���� �ý����� ���� ���� �� �Լ� ---
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
    int32 Score;

    // ������ ������Ű�� �Լ� (������ �Ծ��� �� ȣ��)
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void AddScore(int32 ScoreToAdd);

    UFUNCTION(BlueprintCallable, Category = "Game Clear")
    void SetSpecialCoinCollected(bool bCollected); // �������Ʈ���� ȣ�� �����ϵ��� ����

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Clear")
    float TimeRemaining; // ���� �ð� (�� ����)

    // --- �� ȹ�� ���� (UI ǥ�� ���� ����) ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Clear")
    bool bStar1Achieved;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Clear")
    bool bStar2Achieved;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Clear")
    bool bStar3Achieved;

    // --- Ŭ���� ���� Ȯ�� �Լ� ---
    UFUNCTION(BlueprintCallable, Category = "Game Clear")
    void CheckClearConditions();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Clear")
    bool bIsLevelCleared; // ���� Ŭ���� ������ �����Ǿ����� ����

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Clear")
    bool bHasGameEnded;

private:
    UPROPERTY(EditAnywhere, Category = "Game Clear")
    int32 MaxScore; // ������ �ִ� �� ������ �� (�� 1, 2 ����)

    UPROPERTY(VisibleAnywhere, Category = "Game Clear")
    bool bIsSpecialCoinCollected; // ����� ���� ȹ�� ���� (�� 3 ����)

    UPROPERTY(EditAnywhere, Category = "Game Clear")
    float LevelMaxTime; // ������ �ִ� �ð� (�� ����) (�� 2 ����)



    // --- Ÿ�̸� ���� �Լ� ---
    void UpdateGameTimer(); // �� �ʸ��� ȣ��� Ÿ�̸� �Լ�
    FTimerHandle GameTimerHandle; // Ÿ�̸� �ڵ�

    // --- ������ ������ ���� ���� ---
    UPROPERTY(EditAnywhere, Category = "Movement|Jump")
    float DefaultJumpZVelocity; // ĳ������ ���� JumpZVelocity�� ������ ����

    UPROPERTY(EditAnywhere, Category = "Movement|Jump")
    float MaxJumpZVelocityMultiplier; // �ִ� �ӵ��� �� JumpZVelocity�� �ִ� ����

};
