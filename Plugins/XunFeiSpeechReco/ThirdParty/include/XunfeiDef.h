#pragma once

#include "msp_cmn.h"
#include "msp_errors.h"
#include "qisr.h"
#include <functional>

#define	BUFFER_SIZE	4096
#define MAX_GRAMMARID_LEN   32
#define MAX_PARAMS_LEN      1024
#define SAMPLE_RATE_16K     16000 

static const char* ASR_RES_PATH = "\\res\\asr\\common.jet";  //�����﷨ʶ����Դ·��
#ifdef _WIN64
static const char* GRM_BUILD_PATH = "\\res\\asr\\GrmBuilld_x64";  //���������﷨ʶ�������������ݱ���·��
#else
static const char* GRM_BUILD_PATH = "\\res\\asr\\GrmBuilld";  //���������﷨ʶ�������������ݱ���·��
#endif

static const char* GRM_FILE = "/res/call.bnf"; //��������ʶ���﷨�������õ��﷨�ļ�
typedef struct _UserData {
    int     build_fini;  //��ʶ�﷨�����Ƿ����
    int     update_fini; //��ʶ���´ʵ��Ƿ����
    int     errcode; //��¼�﷨��������´ʵ�ص�������
    char    grammar_id[MAX_GRAMMARID_LEN]; //�����﷨�������ص��﷨ID
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