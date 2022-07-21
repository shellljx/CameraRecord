//
// Created by 李金祥 on 2022/7/20.
//

#include "egl_core.h"

EGLCore::EGLCore() : display_(EGL_NO_DISPLAY), config_(nullptr), context_(EGL_NO_CONTEXT) {

}

EGLCore::~EGLCore() {

}

int EGLCore::init(EGLContext shared_context) {
    EGLint numConfigs;
    const EGLint attributes[] = {
            EGL_BUFFER_SIZE, 32,
            EGL_ALPHA_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,
            EGL_WINDOW_BIT,
            EGL_NONE
    };
    if ((display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        return -1;
    }
    if (!eglInitialize(display_, 0, 0)) {
        return -2;
    }

    if (!eglChooseConfig(display_, attributes, &config_, 1, &numConfigs)) {
        release();
        return -3;
    }

    EGLint eglContextAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, static_cast<EGLint>(2), EGL_NONE};
    context_ = eglCreateContext(display_, config_, shared_context, eglContextAttributes);
    if (context_ == nullptr) {
        release();
        return -4;
    }
    return 0;
}

void EGLCore::release() {
    if (EGL_NO_DISPLAY != display_ || EGL_NO_CONTEXT != context_) {
        eglMakeCurrent(display_, EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(display_, context_);
        eglTerminate(display_);
        eglReleaseThread();
    }
    display_ = EGL_NO_DISPLAY;
    context_ = EGL_NO_CONTEXT;
}

EGLSurface EGLCore::createWindowSurface(ANativeWindow *pWindow) {
    EGLSurface surface = EGL_NO_SURFACE;
    EGLint format;
    if (pWindow == nullptr) {
        return surface;
    }

    if (!eglGetConfigAttrib(display_, config_, EGL_NATIVE_VISUAL_ID, &format)) {
        release();
        return surface;
    }

    ANativeWindow_setBuffersGeometry(pWindow, 0, 0, format);
    if (!(surface = eglCreateWindowSurface(display_, config_, pWindow, nullptr))) {

    }
    return surface;
}

int EGLCore::MakeCurrent(EGLSurface pVoid) {
    return eglMakeCurrent(display_, pVoid, pVoid, context_);
}

bool EGLCore::swapBuffers(EGLSurface pVoid) {
    return eglSwapBuffers(display_, pVoid);
}
