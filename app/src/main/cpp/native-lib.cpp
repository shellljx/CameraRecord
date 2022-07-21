#include <jni.h>
#include <string>
#include "recorder.h"
#include "jni_env.h"

static Recorder* recorder = nullptr;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    setJavaVm(vm);
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gmail_shellljx_camerarecord_Recorder_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}extern "C"

JNIEXPORT void JNICALL
Java_com_gmail_shellljx_camerarecord_Recorder_surfaceCreateJNI(JNIEnv *env, jobject thiz, jobject surface) {
    recorder = new Recorder(thiz);
    recorder->OnSurfaceCreat(surface);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_gmail_shellljx_camerarecord_Recorder_onFrameAvailable(JNIEnv *env, jobject thiz) {
    recorder->DrawFrame();
}
