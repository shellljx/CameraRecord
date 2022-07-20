//
// Created by 李金祥 on 2022/7/20.
//

#ifndef CAMERARECORD_JNI_ENV_H
#define CAMERARECORD_JNI_ENV_H

#include <jni.h>

int setJavaVm(void* vm);

JavaVM* getJavaVm();

int getJniEnv(JNIEnv** env);

void detachThreadEnv();

#endif //CAMERARECORD_JNI_ENV_H
