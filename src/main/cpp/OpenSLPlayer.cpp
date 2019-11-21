//
// Created by bian on 2019/11/21.
//


#include <android/log.h>

#define LOG_TAG "OpenSLPlayer"
#ifdef __cplusplus
extern "C" {

#include "include/com_flyscale_chapter_4_2_2_opensl_OpenSLPlayer.h"
#include "AudioPlayer.h"
#endif

JNIEXPORT jstring JNICALL Java_com_flyscale_chapter_14_12_12_1opensl_OpenSLPlayer_getStringFromJNI
        (JNIEnv *env, jobject, jstring jstr) {
    const char *str = env->GetStringUTFChars(jstr, NULL);
    LOGI("Get message from Java:%s", str);
    return env->NewStringUTF("Welcome to JNI world!");
}

AudioPlayer *audioPlayer;
JNIEXPORT void JNICALL Java_com_flyscale_chapter_14_12_12_1opensl_OpenSLPlayer_play
        (JNIEnv *env, jobject jobj, jstring path) {
    const char *utfPath = env->GetStringUTFChars(path, NULL);
    audioPlayer = AudioPlayer::GetInstance();
    int ret = audioPlayer->setDataSource(utfPath);
    if (ret < 0) {
        LOGE("open source file failed!");
    }else {
        audioPlayer->InitOpenSL();
        audioPlayer->Play();
    }
}

#ifdef __cplusplus
}
#endif