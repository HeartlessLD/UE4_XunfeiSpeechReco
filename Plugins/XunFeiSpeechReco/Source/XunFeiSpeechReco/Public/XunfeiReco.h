// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XunfeiDef.h"
#include "XunfeiReco.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpeechResultDelegate, FString, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpeechBeginDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpeechEndDelegate, int32, ResultCode);

UCLASS()
class XUNFEISPEECHRECO_API AXunfeiReco : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXunfeiReco();

	//��ʼ��
	UFUNCTION(BlueprintCallable, Category = XunfeiReco)
		bool Init();

	//��ʼ����
	UFUNCTION(BlueprintCallable, Category = XunfeiReco)
		int32 BeginListening();

	//��������
	UFUNCTION(BlueprintCallable, Category = XunfeiReco)
		int32 EndListening();

	//�ر�ʶ����
	void CloseReco();
private:
	//�����﷨����
	int BuildGrammar();
	//�����ص�
	static int BuildGrammar_Callback(int ecode, const char* info, void* udata);

	//���ò���
	void RecoConfig();
	//����˷��н���ʶ��
	void RecoFromMic(const char* session_begin_params);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//ʶ�����ص�
	UPROPERTY(BlueprintAssignable)
		FSpeechResultDelegate OnSpeechResult;

	//ʶ��ʼ�ص�
	UPROPERTY(BlueprintAssignable, Category = XunfeiReco)
		FSpeechBeginDelegate OnSpeechBegin;


	//ʶ������ص�
	UPROPERTY(BlueprintAssignable, Category = XunfeiReco)
		FSpeechEndDelegate OnSpeechEnd;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
private:
	UserData asr_data;
	speech_rec asr;
	speech_rec_notifier recnotifier;

	//static char* g_result;


};
