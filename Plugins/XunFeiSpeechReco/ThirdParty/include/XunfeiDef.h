#pragma once

#include "msp_cmn.h"
#include "msp_errors.h"
#include "qisr.h"
#include <functional>

#define	BUFFER_SIZE	4096
#define MAX_GRAMMARID_LEN   32
#define MAX_PARAMS_LEN      1024
#define SAMPLE_RATE_16K     16000 

static const char* ASR_RES_PATH = "\\res\\asr\\common.jet";  //离线语法识别资源路径
#ifdef _WIN64
static const char* GRM_BUILD_PATH = "\\res\\asr\\GrmBuilld_x64";  //构建离线语法识别网络生成数据保存路径
#else
static const char* GRM_BUILD_PATH = "\\res\\asr\\GrmBuilld";  //构建离线语法识别网络生成数据保存路径
#endif

static const char* GRM_FILE = "/res/call.bnf"; //构建离线识别语法网络所用的语法文件
typedef struct _UserData {
    int     build_fini;  //标识语法构建是否完成
    int     update_fini; //标识更新词典是否完成
    int     errcode; //记录语法构建或更新词典回调错误码
    char    grammar_id[MAX_GRAMMARID_LEN]; //保存语法构建返回的语法ID
}UserData;

struct speech_rec_notifier {
	std::function<void(const char* result, char is_last)> on_result;
	std::function<void()> on_speech_begin;
	std::function<void(int reason)> on_speech_end;
	/* 0 if VAD.  others, error : see E_SR_xxx and msp_errors.h  */
};

struct speech_rec {
	enum sr_audsrc aud_src;  /* from mic or manual  stream write */
	struct speech_rec_notifier notif;
	const char* session_id;
	int ep_stat;
	int rec_stat;
	int audio_status;
	struct recorder* recorder;
	volatile int state;
	char* session_begin_params;
};