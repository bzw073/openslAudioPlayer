//
// Created by bian on 2019/11/21.
//

#ifndef NDKPROJECT_AUDIOPLAYER_H
#define NDKPROJECT_AUDIOPLAYER_H
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
extern "C"{
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
};
class AudioPlayer{
private:
    static AudioPlayer *INSTANCE;
    SLObjectItf engineObj;  //引擎对象接口
    SLEngineItf engineEngine;
    SLObjectItf outputMixObj;
    SLDataLocator_AndroidSimpleBufferQueue outputLocator;
    SLObjectItf audioPlayerObj;

    SLPlayItf audioPlayerPlay;

    AudioPlayer();



public:
    static AudioPlayer* GetInstance();

    int32_t setDataSource(const char* );
    SLresult InitOpenSL();
    int Play();
    int Stop();
    SLuint32 GetSampleSize();
    ~AudioPlayer();
};
#endif //NDKPROJECT_AUDIOPLAYER_H
