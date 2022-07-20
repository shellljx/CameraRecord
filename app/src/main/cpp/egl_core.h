//
// Created by 李金祥 on 2022/7/20.
//

#ifndef CAMERARECORD_EGL_CORE_H
#define CAMERARECORD_EGL_CORE_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <KHR/khrplatform.h>

class EGLCore {
public:
    explicit EGLCore();
    ~EGLCore();

    int init(EGLContext shared_context);

    EGLContext getContext(){
        return context_;
    }

    EGLDisplay getDisplay(){
        return display_;
    }

    void release();

    EGLSurface createWindowSurface(ANativeWindow *pWindow);

    int MakeCurrent(EGLSurface pVoid);

private:
    //显示设备
    EGLDisplay display_;
    //配置
    EGLConfig config_;
    //opengl 上下文
    EGLContext context_;
};


#endif //CAMERARECORD_EGL_CORE_H
