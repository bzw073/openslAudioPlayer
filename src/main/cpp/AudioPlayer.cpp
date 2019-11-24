//
// Created by bian on 2019/11/21.
//

#define LOG_TAG "AudioPlayer"

#include <SLES/OpenSLES_Android.h>
#include <pthread.h>
#include <__mutex_base>
#include <android/log.h>
#include "AudioPlayer.h"

void *readPCMBuffer;
FILE *pcmFile;
uint8_t *tempBuffer;
SLAndroidSimpleBufferQueueItf outputBufferQueueInterface;

AudioPlayer::AudioPlayer() {
    tempBuffer = (uint8_t *) malloc(44100 * 2 * 2);
}

AudioPlayer::~AudioPlayer() {
}

SLuint32 getPcmData(void **pcm, FILE *pcmFile, uint8_t *out_buffer) {

    while (!feof(pcmFile)) {
        //因为PCM采样率为44100,采样精度为16BIT，所以一次读取2秒钟的采样
        size_t size = fread(out_buffer, 1, 44100 * 2 * 2, pcmFile);
        *pcm = out_buffer;
        return size;
    }
    return 0;
}

/**
 * 当outputBufferQueueInterface中的数据消耗完就会触发回调
 * @param bufferQueue
 * @param pContext
 */
void PlayCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *pContext) {
    LOGI("PlayCallback");
    //获取数据
    SLuint32 size = getPcmData(&readPCMBuffer, pcmFile, tempBuffer);
    LOGI("PlayCallback, size=%d", size);
    if (NULL != readPCMBuffer && size > 0) {
        SLresult result = (*outputBufferQueueInterface)->Enqueue(outputBufferQueueInterface,
                                                                 readPCMBuffer, size);
    }
}

SLresult AudioPlayer::InitOpenSL() {
    LOGI("InitOpenSL");
    //1.1获取引擎对象接口
    SLresult result = slCreateEngine(&engineObj, 0, 0, 0, 0, 0);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("slCreateEngine failed, result=%d", result);
        return result;
    }
    //1.2实例化引擎对象接口
    result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("engineObject Realize failed, result=%d", result);
        return result;
    }
    //2.1获取SLEngineItf类型对象接口,后续操作将会使用这个接口
    result = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engineEngine);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("engineObj GetInterface failed, result=%d", result);
        return result;
    }
    //3.1创建音频输出混音对象接口
    const SLInterfaceID ids[] = {SL_IID_VOLUME};
    const SLboolean req[] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObj, 0, ids, req);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("CreateOutputMix failed, ret=%d", result);
        return result;
    }
    //3.2实例化音频输出混音对象接口
    result = (*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) {
        LOGD("Realize outputMixObj failed, result=%d", result);
        return result;
    }
    //4.1配置Buffer Queue参数
    outputLocator = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    //4.2设置音频格式
    SLDataFormat_PCM outputFormat = {SL_DATAFORMAT_PCM,         //指定PCM格式
                                     2,                         //通道个数
                                     SL_SAMPLINGRATE_44_1,           //采样率
                                     SL_PCMSAMPLEFORMAT_FIXED_16,//采样精度
                                     SL_PCMSAMPLEFORMAT_FIXED_16,//窗口大小
                                     SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//通道掩码
                                     SL_BYTEORDER_LITTLEENDIAN  //字节序：小端
    };
    //4.3输出源
    SLDataSource outputSource = {&outputLocator, &outputFormat};
    //4.4输出管道
    SLDataLocator_OutputMix outputMixLocator = {SL_DATALOCATOR_OUTPUTMIX, outputMixObj};
    SLDataSink outputSink = {&outputMixLocator, NULL};

    //5.1获取播放器对象接口
    const SLInterfaceID outputInterfaces[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean requireds[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine,
                                       &audioPlayerObj,
                                       &outputSource,//输出源
                                       &outputSink,//输出管道
                                       1,//接口个数
                                       outputInterfaces,//输出接口
                                       requireds);  //接口配置
    if (result != SL_RESULT_SUCCESS) {
        LOGD("CreateAudioPlayer() failed,result=%d", result);
        return result;
    }
    //5.2实例化播放器对象接口
    result = (*audioPlayerObj)->Realize(audioPlayerObj, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("audioPlayerObj Realize failed,result=%d", result);
        return result;
    }
    //6.1获取音频输出对象接口
    result = (*audioPlayerObj)->GetInterface(audioPlayerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                    &outputBufferQueueInterface);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("audioPlayerObj SL_IID_ANDROIDSIMPLEBUFFERQUEUE GetInterface failed,result=%d", result);
        return result;
    }
    //7.1获取播放器播放对象接口
    result = (*audioPlayerObj)->GetInterface(audioPlayerObj, SL_IID_PLAY, &audioPlayerPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGD("audioPlayerObj SL_IID_PLAY GetInterface failed,result=%d", result);
        return result;
    }
    //8.1设置回调
    result = (*outputBufferQueueInterface)->RegisterCallback(outputBufferQueueInterface,
                                                    PlayCallback,
                                                    this);

    return 0;
}

int AudioPlayer::Play() {
    LOGI("Play");
    //9设置为播放状态
    (*audioPlayerPlay)->SetPlayState(audioPlayerPlay, SL_PLAYSTATE_PLAYING);

    LOGI("setPlayerState:SL_PLAYSTATE_PLAYING");
    //10.调用回调函数，启动播放
    PlayCallback(outputBufferQueueInterface, this);
    return 0;
}

int AudioPlayer::Stop() {
    //11.停止播放
    (*audioPlayerPlay)->SetPlayState(audioPlayerPlay, SL_PLAYSTATE_STOPPED);
    return 0;
}


int32_t AudioPlayer::setDataSource(const char *path) {
    pcmFile = fopen(path, "r");
    if (pcmFile == NULL) {
        LOGE("open path=%s failed.", path);
        return -1;
    }
    return 0;
}

AudioPlayer *AudioPlayer::INSTANCE = new AudioPlayer();

AudioPlayer *AudioPlayer::GetInstance() {
    return INSTANCE;
}

