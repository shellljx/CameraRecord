//
// Created by 李金祥 on 2022/7/20.
//

#include "jni_env.h"
#include <pthread.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_key_t current_env;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static JavaVM *javaVm;

int setJavaVm(void *vm) {
    pthread_mutex_lock(&lock);
    if (javaVm == nullptr) {
        javaVm = reinterpret_cast<JavaVM *>(vm);
    } else {
        return -1;
    }
    pthread_mutex_unlock(&lock);
    return 0;
}

JavaVM *getJavaVm() {
    void *vm;
    pthread_mutex_lock(&lock);
    vm = javaVm;
    pthread_mutex_unlock(&lock);
    return reinterpret_cast<JavaVM *>(vm);
}

int getJniEnv(JNIEnv **env) {
    JavaVM *vm = getJavaVm();
    int ret = (*vm).GetEnv((void **) env, JNI_VERSION_1_6);
    if (ret == JNI_EDETACHED) {
        if ((*vm).AttachCurrentThread(env, nullptr) != JNI_OK) {
            *env = nullptr;
            return -1;
        }
    }
    return ret;
}

void detachThreadEnv(){
    JavaVM* vm = getJavaVm();
    (*vm).DetachCurrentThread();
}
