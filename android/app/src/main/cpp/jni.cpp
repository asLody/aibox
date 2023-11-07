#include <jni.h>
#include <string>
#include <usbg/usbg.h>

extern "C" JNIEXPORT jstring JNICALL
Java_aaiiaaii_io_aibox_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_aaiiaaii_io_aibox_MainActivity_mouseEvent(JNIEnv *env, jobject thiz, jfloat x, jfloat y,
                                               jint action) {
    // TODO: implement mouseEvent()
}