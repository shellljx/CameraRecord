//
// Created by 李金祥 on 2022/7/20.
//

#include "recorder.h"
#include "jni_env.h"


Recorder::Recorder(jobject object):renderSurface_(EGL_NO_SURFACE) {

    JNIEnv* env = nullptr;
    int ret = getJniEnv(&env);
    if (env!= nullptr){
        recorder_object_ = env->NewGlobalRef(object);
    }

    eglCore = new EGLCore();
    ret = eglCore->init(nullptr);
    if (ret!=0){
        eglCore->release();
        delete eglCore;
        eglCore = nullptr;
    }
}

Recorder::~Recorder() {
    JNIEnv* env = nullptr;
    int ret = getJniEnv(&env);
    if (env!= nullptr){
        env->DeleteGlobalRef(recorder_object_);
        recorder_object_ = nullptr;
    }
    eglCore->release();
    delete eglCore;
}


void Recorder::OnSurfaceCreat(jobject surface) {
    JNIEnv* env = nullptr;
    int ret = getJniEnv(&env);
    if (env != nullptr){
        nativeWindow_ = ANativeWindow_fromSurface(env, surface);
        createEGLSurfaceInternal();
    }
}

int Recorder::createEGLSurfaceInternal() {
    if (eglCore == nullptr){
        return -1;
    }

    if (nativeWindow_ == nullptr){
        return -2;
    }
    renderSurface_ = eglCore->createWindowSurface(nativeWindow_);
    if (renderSurface_ == EGL_NO_SURFACE){
        return -3;
    }

    auto ret = eglCore->MakeCurrent(renderSurface_);

    if (previewTextureId_ == 0){
        glGenTextures(1, &previewTextureId_);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, previewTextureId_);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    callJavaStartPreview();
    return ret;
}

void Recorder::callJavaStartPreview() {
    if (recorder_object_ == nullptr){
        return;
    }
    JNIEnv* env = nullptr;
    getJniEnv(&env);
    if (env!= nullptr){
        auto clazz = env->GetObjectClass(recorder_object_);
        jmethodID startPreviewId = env->GetMethodID(clazz, "startPreviewFromNative", "(I)V");
        if (startPreviewId != nullptr){
            env->CallVoidMethod(recorder_object_, startPreviewId, previewTextureId_);
        }
        env->DeleteLocalRef(clazz);
    }
}
