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

	//初始化
	UFUNCTION(BlueprintCallable, Category = XunfeiReco)
		bool Init();

	//开始监听
	UFUNCTION(BlueprintCallable, Category = XunfeiReco)
		int32 BeginListening();

	//结束监听
	UFUNCTION(BlueprintCallable, Category = XunfeiReco)
		int32 EndListening();

	//关闭识别功能
	void CloseReco();
private:
	//构建语法数据
	int BuildGrammar();
	//构建回调
	static int BuildGrammar_Callback(int ecode, const char* info, void* udata);

	//配置参数
	void RecoConfig();
	//从麦克风中进行识别
	void RecoFromMic(const char* session_begin_params);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//识别结果回调
	UPROPERTY(BlueprintAssignable)
		FSpeechResultDelegate OnSpeechResult;

	//识别开始回调
	UPROPERTY(BlueprintAssignable, Category = XunfeiReco)
		FSpeechBeginDelegate OnSpeechBegin;


	//识别结束回调
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
