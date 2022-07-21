//
// Created by 李金祥 on 2022/7/20.
//

#ifndef CAMERARECORD_RECORDER_H
#define CAMERARECORD_RECORDER_H

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "egl_core.h"

class Recorder {
public:
    explicit Recorder(jobject object);
    ~Recorder();

    void OnSurfaceCreat(jobject surface);

    void DrawFrame();

private:
    int createEGLSurfaceInternal();
    void callJavaStartPreview();

private:
    jobject recorder_object_ = nullptr;
    ANativeWindow *nativeWindow_ = nullptr;
    EGLCore* eglCore = nullptr;
    EGLSurface renderSurface_;

    GLuint previewTextureId_;

    void callJavaUpdateTexImage();

    void callJavaTextureMatrix();
};


#endif //CAMERARECORD_RECORDER_H
