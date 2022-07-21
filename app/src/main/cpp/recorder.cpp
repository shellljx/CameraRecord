//
// Created by 李金祥 on 2022/7/20.
//

#include "recorder.h"
#include "jni_env.h"
#include "ShaderLoader.h"
#include "shaders.h"

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
void Recorder::callJavaUpdateTexImage() {
    if (recorder_object_ == nullptr){
        return;
    }
    JNIEnv* env = nullptr;
    getJniEnv(&env);
    if (env!= nullptr){
        auto clazz = env->GetObjectClass(recorder_object_);
        jmethodID startPreviewId = env->GetMethodID(clazz, "updateTexImageFromNative", "()V");
        if (startPreviewId != nullptr){
            env->CallVoidMethod(recorder_object_, startPreviewId, previewTextureId_);
        }
        env->DeleteLocalRef(clazz);
    }
}

float * texture_matrix_ = new float[16];

void Recorder::callJavaTextureMatrix() {
    if (recorder_object_ == nullptr){
        return;
    }
    JNIEnv* env = nullptr;
    getJniEnv(&env);
    if (env!= nullptr){
        auto clazz = env->GetObjectClass(recorder_object_);
        jmethodID startPreviewId = env->GetMethodID(clazz, "getTextureMatrix", "()[F");
        if (startPreviewId != nullptr){
            auto jobj = env->CallObjectMethod(recorder_object_, startPreviewId);

            auto matrixj = reinterpret_cast<jfloatArray>(jobj);
            float* matrix = env->GetFloatArrayElements(matrixj, JNI_FALSE);
            memcpy(texture_matrix_, matrix, sizeof(float)*16);
            env->ReleaseFloatArrayElements(matrixj, matrix, 0);
            env->DeleteLocalRef(jobj);
        }
        env->DeleteLocalRef(clazz);
    }
}
/// 默认顶点坐标
static float DEFAULT_VERTEX_COORDINATE[] = {
        -1.F, -1.F,
        1.F, -1.F,
        -1.F, 1.F,
        1.F, 1.F
};

/// 默认纹理坐标
static float DEFAULT_TEXTURE_COORDINATE[] = {
        0.F, 0.F,
        1.F, 0.F,
        0.F, 1.F,
        1.F, 1.F
};


GLuint program = 0;
GLuint frameBuffer = 0;
GLuint frameBufferTexture = 0;
void Recorder::DrawFrame() {
    if (renderSurface_ == EGL_NO_SURFACE || nativeWindow_ == nullptr){
        return;
    }
    eglCore->MakeCurrent(renderSurface_);
    callJavaUpdateTexImage();
    callJavaTextureMatrix();
    glViewport(0.f,500.f,1080.f, 1080.f);
    glClearColor(0.f, 0.f, 0.0f, 1.0f);

    if (program == 0){
        program = loadShader(DEFAULT_VERTEX_SHADER, DEFAULT_OES_FRAGMENT_SHADER);
    }


    glUseProgram(program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto positionAttr = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionAttr);
    glVertexAttribPointer(positionAttr, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat),DEFAULT_VERTEX_COORDINATE);
    auto textCoordAttr = glGetAttribLocation(program, "inputTextureCoordinate");
    glEnableVertexAttribArray(textCoordAttr);
    glVertexAttribPointer(textCoordAttr,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat), DEFAULT_TEXTURE_COORDINATE);
    auto matrixLocation = glGetUniformLocation(program,"textureMatrix");
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, texture_matrix_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, previewTextureId_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(positionAttr);
    glDisableVertexAttribArray(textCoordAttr);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);

    if (!eglCore->swapBuffers(renderSurface_)){
    }
}
