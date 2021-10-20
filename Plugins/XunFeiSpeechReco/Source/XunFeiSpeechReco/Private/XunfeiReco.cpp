// Fill out your copyright notice in the Description page of Project Settings.


#include "XunfeiReco.h"
#include "Interfaces/IPluginManager.h"
#include "speech_recognizer.h"
#include <string>

using namespace std;
DECLARE_LOG_CATEGORY_EXTERN(XunfeiReco, Log, All);
DEFINE_LOG_CATEGORY(XunfeiReco);


// Sets default values
AXunfeiReco::AXunfeiReco()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

bool AXunfeiReco::Init()
{
	//
	const char* login_config = "appid = xxx"; //登录参数

	int ret = 0;

	ret = MSPLogin(NULL, NULL, login_config); //第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret) {
		UE_LOG(XunfeiReco, Error, TEXT("登录失败：%d"), ret);
		return false;
	}
	UE_LOG(XunfeiReco, Log, TEXT("开始构建离线识别语法网络"));
	if (MSP_SUCCESS != BuildGrammar())
	{
		return false;
	}
	while (1 != asr_data.build_fini)
		FPlatformProcess::Sleep(0.3);

	if (MSP_SUCCESS != asr_data.errcode)
	{
		UE_LOG(XunfeiReco, Error, TEXT("构建离线网络失败"));
		return false;
	}
	UE_LOG(XunfeiReco, Log, TEXT("构建离线识别语法网络成功"));

	RecoConfig();
	return true;
}

int32 AXunfeiReco::BeginListening()
{
	int errcode = sr_start_listening(&asr);
	if (0 == errcode) {
		UE_LOG(XunfeiReco, Log, TEXT("开始监听麦克风"));
	}
	else
	{
		switch (errcode)
		{
		case E_SR_ALREADY:
			UE_LOG(XunfeiReco, Error, TEXT("麦克风已经开始监听"));
			break;
		case E_SR_RECORDFAIL:
			UE_LOG(XunfeiReco, Error, TEXT("start_record failed! code = %d"), errcode);
			break;
		default:
			UE_LOG(XunfeiReco, Error, TEXT("QISRSessionBegin failed code = %d"), errcode);
		}
		
	}
	return errcode;
}

int AXunfeiReco::EndListening()
{
	int errcode = sr_stop_listening(&asr);
	if (errcode == 0) {
		UE_LOG(XunfeiReco, Log, TEXT("结束监听麦克风"));
	}
	else
	{
		UE_LOG(XunfeiReco, Log, TEXT("结束监听麦克风失败 %d"), errcode);
	}
	return errcode;
}

void AXunfeiReco::CloseReco()
{
	MSPLogout();
	UE_LOG(XunfeiReco, Log, TEXT("关闭讯飞识别"));
}

int AXunfeiReco::BuildGrammar()
{
	FILE* grm_file = NULL;
	unsigned int grm_cnt_len = 2;
	char grm_build_params[MAX_PARAMS_LEN] = { NULL };
	int ret = 0;

	FString BaseDir = IPluginManager::Get().FindPlugin("XunfeiSpeechReco")->GetBaseDir() + TEXT("/ThirdParty");
	FString BnfFilePath = BaseDir +  + GRM_FILE;

	if (0 != fopen_s(&grm_file, TCHAR_TO_UTF8 (*BnfFilePath), "rb")) {
		UE_LOG(XunfeiReco, Error, TEXT("打开 %s 文件失败"), *BnfFilePath);
		return -1;
	}

	fseek(grm_file, 0, SEEK_END);
	grm_cnt_len = ftell(grm_file);
	fseek(grm_file, 0, SEEK_SET);
	char* grm_content = new char[grm_cnt_len + 1];

	if (NULL == grm_content)
	{
		UE_LOG(XunfeiReco, Error, TEXT("内存分配失败"));
		fclose(grm_file);
		grm_file = NULL;
		return -1;
	}

	fread((void*)grm_content, 1, grm_cnt_len, grm_file);

	grm_content[grm_cnt_len] = '\0';
	fclose(grm_file);
	grm_file = NULL;

	FString GRMPath = BaseDir + GRM_BUILD_PATH;
	GRMPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);
	FString JetPath = TEXT("fo|") + FPaths::ConvertRelativePathToFull(BaseDir) + ASR_RES_PATH;
	JetPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);
	
	
	_snprintf_s(grm_build_params, MAX_PARAMS_LEN - 1,
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, ",
		TCHAR_TO_UTF8(*JetPath),
		SAMPLE_RATE_16K,
		TCHAR_TO_UTF8(*GRMPath)
	);
	UE_LOG(XunfeiReco, Log, TEXT("GRMPath ===%s"), *GRMPath);
	UE_LOG(XunfeiReco, Log, TEXT("JetPath ===%s"), *JetPath);
	ret = QISRBuildGrammar("bnf", grm_content, grm_cnt_len, grm_build_params, &AXunfeiReco::BuildGrammar_Callback, &asr_data);

	delete grm_content;
	grm_content = NULL;

	return ret;
}

int AXunfeiReco::BuildGrammar_Callback(int ecode, const char* info, void* udata)
{
	UserData* grm_data = (UserData*)udata;
	if (NULL != grm_data) {
		grm_data->build_fini = 1;
		grm_data->errcode = ecode;
	}
	if (MSP_SUCCESS == ecode && NULL != info) {
		UE_LOG(XunfeiReco, Log, TEXT("构建语法成功！ 语法ID:%s"), info);
		if (NULL != grm_data)
			_snprintf_s(grm_data->grammar_id, MAX_GRAMMARID_LEN - 1, info);
	}
	else
	{
		UE_LOG(XunfeiReco, Error, TEXT("构建语法失败！%d 请检查 ASR_RES_PATH 等相关语法文件路径"), ecode);
	}

	return 0;
}

void AXunfeiReco::RecoConfig()
{
	//配置参数
	char asr_params[MAX_PARAMS_LEN] = { NULL };
	const char* rec_rslt = NULL;
	const char* session_id = NULL;
	const char* asr_audiof = NULL;
	FILE* f_pcm = NULL;
	char* pcm_data = NULL;
	long pcm_count = 0;
	long pcm_size = 0;
	int last_audio = 0;
	int aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	int ep_status = MSP_EP_LOOKING_FOR_SPEECH;
	int rec_status = MSP_REC_STATUS_INCOMPLETE;
	int rss_status = MSP_REC_STATUS_INCOMPLETE;
	int errcode = -1;
	int aud_src = 0;

	FString BaseDir = IPluginManager::Get().FindPlugin("XunfeiSpeechReco")->GetBaseDir() + TEXT("/ThirdParty");
	FString GRMPath = BaseDir + GRM_BUILD_PATH;
	GRMPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);
	FString JetPath = TEXT("fo|") + BaseDir + ASR_RES_PATH;
	JetPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);
	
	//离线语法识别参数设置
	_snprintf_s(asr_params, MAX_PARAMS_LEN - 1,
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, local_grammar = %s, \
		result_type = xml, result_encoding = GB2312, ",
		TCHAR_TO_UTF8(*JetPath),
		SAMPLE_RATE_16K,
		TCHAR_TO_UTF8(*GRMPath),
		asr_data.grammar_id
	);
	RecoFromMic(asr_params);
}
PRAGMA_DISABLE_OPTIMIZATION


string GBKToUTF8(const std::string& strGBK)
{
	string strOutUTF8 = "";
	WCHAR* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

void AXunfeiReco::RecoFromMic(const char* session_begin_params)
{
	//获得识别分析结果回调
	recnotifier.on_result = [this](const char* result, char is_last)
	{
		std::string Temp(result);
		std::string Temp2 = GBKToUTF8(Temp);
		
		FString Result(UTF8_TO_TCHAR(Temp2.c_str()));
		if (OnSpeechResult.IsBound())
		{
			OnSpeechResult.Broadcast(Result);
		}
		
	};
	//开始麦克风输入回调
	recnotifier.on_speech_begin = [this]()
	{
		if (OnSpeechBegin.IsBound())
		{
			OnSpeechBegin.Broadcast();
		}

	};
	//结束麦克风输入回调
	recnotifier.on_speech_end = [this](int reason)
	{
		if (reason == END_REASON_VAD_DETECT)
			printf("\nSpeaking done \n");
		else
			UE_LOG(XunfeiReco, Error, TEXT("麦克风识别失败 %d"), reason);
		if (OnSpeechEnd.IsBound())
		{
			OnSpeechEnd.Broadcast(reason);
		}
	};

	//初始化
	int errCode = sr_init(&asr, session_begin_params, SR_MIC, DEFAULT_INPUT_DEVID, &recnotifier);
	if (errCode) {
		UE_LOG(XunfeiReco, Error, TEXT("麦克风初始化失败"));
		return;
	}
}
PRAGMA_ENABLE_OPTIMIZATION
// Called when the game starts or when spawned
void AXunfeiReco::BeginPlay()
{
	Super::BeginPlay();
	
}

void AXunfeiReco::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CloseReco();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
//void AXunfeiReco::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

