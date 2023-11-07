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